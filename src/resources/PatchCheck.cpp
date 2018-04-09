#include "PatchCheck.h"
#include "GameSignals.h"
#include "md5.h"
#include <SFML/Network.hpp>
#include <fstream>
#include <curl/curl.h>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <cstring>

using std::to_string;
using std::cout;
using std::endl;

#ifdef __APPLE__
#include "ResourcePath.hpp"
#include "RunAsAdmin.hpp"
#elif __WIN32
#include "EmptyResourcePath.h"
#include <windows.h>
#include <shellapi.h>
#else
#include "EmptyResourcePath.h"
#endif

PatchCheck::PatchCheck() {
	connectSignal("ApplyPatch", &PatchCheck::apply, this);
}

std::string PatchCheck::exec(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

void PatchCheck::parseJson(const std::string& jsonString) {
	size_t from = 0, to = 0;
	j1.clear();
	while (true) {
		if (from >= jsonString.size())
			break;
		to = jsonString.find(';', from);
		if (to == std::string::npos)
			break;
		std::string key = jsonString.substr(from, to-from);
		from=to+1;
		to = jsonString.find(';', from);
		std::string value = jsonString.substr(from, to-from);
		from=to+1;
		j1[key] = value;
	}
}

void PatchCheck::check(int version) {
	#ifdef _WIN32
		parseJson(sendPost("/update/check_for_patch.php", "version=" + to_string(version) + "&os=win"));
	#elif __APPLE__
		parseJson(sendPost("/update/check_for_patch.php", "version=" + to_string(version) + "&os=mac"));
	#else
		parseJson(sendPost("/update/check_for_patch.php", "version=" + to_string(version) + "&os=linux"));
	#endif
	if (cancelCheck()) return;
	if (j1.find("latest") != j1.end()) {
		status=4;
		return;
	}
	if (j1.find("update") == j1.end()) {
		status=-2;
		return;
	}

	j1.erase("update");
	changelog = "";
	if (j1.find("changelog") != j1.end()) {
		changelog = j1["changelog"];
		j1.erase("changelog");
	}
	files_total = j1.size();
	files_downloaded = 0;
	status=5;

	#ifdef __APPLE__
	tmpDir = exec("echo $TMPDIR");
	tmpDir.pop_back();
	#elif __WIN32
	char tmp[500];
	GetTempPath(500, tmp);
	tmpDir = std::string(tmp);
	#endif

	for (const auto file : j1) {
		if (cancelCheck()) return;
		int result = download_file(file.first);
		if (result) {
			status=result;
			return;
		}
		if (cancelCheck()) return;
		if (!check_md5(file.first, file.second)) {
			status=-4;
			return;
		}
		files_downloaded++;
		status=5;
	}
	if (cancelCheck()) return;

	status=6;
	return;
}

bool PatchCheck::cancelCheck() {
	if (quit) {
		quit=false;
		status=-6;
		return true;
	}
	else
		return false;
}

bool PatchCheck::check_md5(const std::string& file, const std::string& md5) {
	#ifdef __APPLE__
		std::string filename = file.substr(file.find('/')+1);
	#elif __WIN32
		std::string filename = file.substr(file.find('/')+1);
	#else
		std::string filename = "tmp/" + file.substr(file.find('/')+1);
	#endif

	#ifdef _WIN32
		std::string filehash = md5file((tmpDir + filename).c_str());
	#elif __APPLE__
		std::string filehash = md5file((tmpDir + filename).c_str());
	#else
		std::string filehash = md5file(filename.c_str());
	#endif

	if (!filehash.compare(md5))
		return true;
	return false;
}

bool PatchCheck::apply() {
	bool applyAsAdmin=false;
	std::string fullCommand="";
	for (const auto file : j1) {
		std::string filename = file.first;
		filename = filename.substr(filename.find('/')+1);

		size_t pos = filename.find('.');
		std::string copyto;
		if (pos == std::string::npos)
			copyto = "";
		else
			copyto = filename.substr(pos);
		if (!copyto.compare(".exe"))
			copyto = "";
		else if (!copyto.compare(""))
			copyto = ".";
		else if (!copyto.compare(".wav") || !copyto.compare(".ogg"))
			copyto = "sounds";
		else
			copyto = "media";

		#ifdef _WIN32
			std::string copyfrom = tmpDir + filename;
			if (copyto.compare(""))
				copyto = copyto + "\\" + filename;
			std::string cmd = "move /y " + copyfrom + " " + copyto;
			if (!filename.compare("SpeedBlocks.exe"))
				system("move /y SpeedBlocks.exe SpeedBlocks.exe.old");
			if (system(cmd.c_str())) {
				applyAsAdmin=true;
				fullCommand += " " + filename;
			}
		#elif __APPLE__
			std::string copyfrom = tmpDir + filename;
			if (!copyto.compare("."))
				copyto = "../MacOS/";
			else
				copyto = copyto + "/" + filename;
			std::string cmd = "mv -f " + copyfrom + " " + resourcePath() + copyto;
			if (system(cmd.c_str())) {
				applyAsAdmin=true;
				fullCommand+=cmd + ";";
			}
			cmd = "chmod +x " + resourcePath() + "../MacOS/SpeedBlocks";
			system(cmd.c_str());
		#else
			std::string copyfrom = "tmp/" + filename;
			copyto = copyto + "/" + filename;
			std::string cmd = "mv -f " + copyfrom + " " + copyto;
			if (system(cmd.c_str()))
				return false;
			cmd = "chmod +x SpeedBlocks";
			system(cmd.c_str());
		#endif
	}

	if (applyAsAdmin) {
		#ifdef __APPLE__
			fullCommand+= "chmod +x " + resourcePath() + "../MacOS/SpeedBlocks";
			runAsAdmin(fullCommand);
		#elif __WIN32
			char pwd[500];
			GetCurrentDirectory(500, pwd);
			std::string launch(pwd);
			launch+="apply_patch.exe";
			if ((int)ShellExecute(NULL, "runas", "apply_patch.exe", fullCommand.c_str(), pwd, 0) > 32) {
				while (true)
					sf::sleep(sf::seconds(1));
			}
			else
				return false;
		#endif
	}
	return true;
}

std::string PatchCheck::sendPost(const std::string& _request, const std::string& body) {
	sf::Http::Request request(_request, sf::Http::Request::Post);
	request.setBody(body);
    request.setField("Content-Type", "application/x-www-form-urlencoded");
	sf::Http http("http://speedblocks.se");
    return http.sendRequest(request).getBody();
}

struct MemoryStruct {
  char *memory;
  size_t size;
};
 
static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    cout << "not enough memory (realloc returned NULL)" << endl;
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

int PatchCheck::download_file(const std::string& file) {
	CURL *curl;
	CURLcode res;

	struct MemoryStruct chunk;
 
  	chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */ 
  	chunk.size = 0;    /* no data at this point */ 

  	std::string URL = "https://speedblocks.se/update/" + file;

	char * urlstr = new char [URL.size()+1];
	std::strcpy (urlstr, URL.c_str());

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, urlstr);

		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
		headers = curl_slist_append(headers, "Cache-Control: no-cache");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		// Specify callbackfunction to get the response
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

		/* Perform the request, res will get the return code */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			cout << endl;
		}

		curl_slist_free_all(headers);

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	else
		cout << "Curl failed to load" << endl;

	delete[] urlstr;

	if (res == CURLE_OK) {
		#ifdef __APPLE__
		std::string filename = tmpDir + file.substr(file.find('/')+1);
		#elif __WIN32
		std::string filename = tmpDir + file.substr(file.find('/')+1);
		#else
		std::string filename = "tmp/" + file.substr(file.find('/')+1);
		#endif
	    std::ofstream ofile(filename, std::ios::binary);
		if (!ofile.is_open()) {
			std::cout << "Error saving file: " << file << std::endl;
			free(chunk.memory);
			return -5;
		}

	    ofile.write(chunk.memory, chunk.size);
		ofile.close();
		free(chunk.memory);
		return 0;
	}
	else {
		free(chunk.memory);
		return -3;
	}
}