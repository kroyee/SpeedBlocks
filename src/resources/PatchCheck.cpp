#include "PatchCheck.h"
#include <SFML/Network.hpp>
#include <fstream>
#include <curl/curl.h>
using json = nlohmann::json;
using std::to_string;
using std::cout;
using std::endl;

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else
#include "EmptyResourcePath.h"
#endif

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

void PatchCheck::check(int version) {
	#ifdef _WIN32
		j1 = json::parse(sendPost("/update/check_for_patch.php", "version=" + to_string(version) + "&os=win"));
	#elif __APPLE__
		j1 = json::parse(sendPost("/update/check_for_patch.php", "version=" + to_string(version) + "&os=mac"));
	#else
		j1 = json::parse(sendPost("/update/check_for_patch.php", "version=" + to_string(version) + "&os=linux"));
	#endif
	if (j1.find("latest") != j1.end()) {
		status=4;
		return;
	}
	if (j1.find("update") == j1.end()) {
		status=-2;
		return;
	}

	j1.erase("update");
	files_total = j1.size();
	files_downloaded = 0;
	status=5;

	for (auto it = j1.begin(); it != j1.end(); it++) {
		int result = download_file(it.key());
		if (result) {
			status=result;
			return;
		}
		if (!check_md5(it.key(), it.value())) {
			status=-4;
			return;
		}
		files_downloaded++;
		status=5;
	}

	status=6;
	return;
}

bool PatchCheck::check_md5(const std::string& file, const std::string& md5) {
	std::string filename = "tmp/" + file.substr(file.find('/')+1);
	#ifdef _WIN32
		std::string filehash = exec("certutil.exe -hashfile " + filename + " MD5");
		filehash = filehash.substr(filehash.find('\n')+1, 32);
	#elif __APPLE__
		std::string filehash = exec("md5 -r " + resourcePath() + filename);
		filehash = filehash.substr(0, filehash.find(' '));
	#else
		std::string filehash = exec("md5sum -b " + filename);
		filehash = filehash.substr(0, filehash.find(' '));
	#endif

	if (!filehash.compare(md5))
		return true;
	return false;
}

void PatchCheck::apply() {
	for (auto it = j1.begin(); it != j1.end(); it++) {
		std::string filename = it.key();
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
			std::string copyfrom = "tmp\\" + filename;
			if (copyto.compare(""))
				copyto = copyto + "\\" + filename;
			std::string cmd = "move /y " + copyfrom + " " + copyto;
			if (!filename.compare("SpeedBlocks.exe"))
				system("move /y SpeedBlocks.exe SpeedBlocks.exe.old");
			system(cmd.c_str());
		#elif __APPLE__
			std::string copyfrom = "tmp/" + filename;
			if (!copyto.compare("."))
				copyto = "../MacOS/";
			else
				copyto = copyto + "/" + filename;
			std::string cmd = "mv " + resourcePath() + copyfrom + " " + resourcePath() + copyto;
			system(cmd.c_str());
			cmd = "chmod +x " + resourcePath() + "../MacOS/SpeedBlocks";
			system(cmd.c_str());
		#else
			std::string copyfrom = "tmp/" + filename;
			copyto = copyto + "/" + filename;
			std::string cmd = "mv " + copyfrom + " " + copyto;
			system(cmd.c_str());
			cmd = "chmod +x SpeedBlocks";
			system(cmd.c_str());
		#endif
	}
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
		std::string filename = resourcePath() + "tmp/" + file.substr(file.find('/')+1);
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