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
		parseJson(sendPost("/update/check_for_patch.php", "version=" + std::to_string(version) + "&os=win"));
	#elif __APPLE__
		parseJson(sendPost("/update/check_for_patch.php", "version=" + std::to_string(version) + "&os=mac"));
	#else
		parseJson(sendPost("/update/check_for_patch.php", "version=" + std::to_string(version) + "&os=linux"));
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
	#else
	tmpDir = "tmp/";
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
	std::string filehash = md5file((tmpDir + file).c_str());

	if (filehash == md5)
		return true;

	return false;
}

int PatchCheck::apply() {
	bool applyAsAdmin=false;
	std::string fullCommand="";

	for (const auto file : j1) {
		std::string filename = file.first;

		#ifdef _WIN32
			std::string	cmd = "unzip.exe -o '" + tmpDir + filename + "'";
			system("move /y SpeedBlocks.exe SpeedBlocks.exe.old");
			if (system(cmd.c_str())) {
				applyAsAdmin=true;
				fullCommand += " " + filename;
			}
			if (applyAsAdmin)
				system("move /y SpeedBlocks.exe.old SpeedBlocks.exe");
		#elif __APPLE__
			std::string	cmd = "unzip -d '" + resourcePath() + "../' -o '" + tmpDir + filename + "'";
			if (system(cmd.c_str())) {
				applyAsAdmin=true;
				fullCommand+=cmd + ";";
			}
			cmd = "chmod +x '" + resourcePath() + "../MacOS/SpeedBlocks'";
			system(cmd.c_str());
		#else
			std::string cmd = std::string("unzip -o ") + "tmp/" + filename;
			if (system(cmd.c_str()))
				return 0;
			cmd = "chmod +x SpeedBlocks";
			system(cmd.c_str());
		#endif
	}

	if (applyAsAdmin) {
		#ifdef __APPLE__
			fullCommand+= "chmod +x '" + resourcePath() + "../MacOS/SpeedBlocks'";
			return runAsAdmin(fullCommand);
		#elif __WIN32
			if ((int)ShellExecute(NULL, NULL, "helper.exe", fullCommand.c_str(), NULL, 0) > 32)
				return 2;
			else
				return 0;
		#endif
	}
	return 1;
}

std::string PatchCheck::sendPost(const std::string& _request, const std::string& body) {
	sf::Http::Request request(_request, sf::Http::Request::Post);
	request.setBody(body);
    request.setField("Content-Type", "application/x-www-form-urlencoded");
	sf::Http http("http://speedblocks.se");
    return http.sendRequest(request).getBody();
}

int progress_func(void* ptr, curl_off_t TotalToDownload, curl_off_t NowDownloaded, curl_off_t, curl_off_t) {
	if (TotalToDownload <= 0.0)
		return 0;

	static_cast<PatchCheck*>(ptr)->filesize = TotalToDownload;
	static_cast<PatchCheck*>(ptr)->downloaded = NowDownloaded;
	static_cast<PatchCheck*>(ptr)->status = 5;

	return 0;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int PatchCheck::download_file(const std::string& file) {
	CURL *curl;
	CURLcode res;
	FILE* fp;

  std::string URL = "https://speedblocks.se/update/new/" + file;

	curl = curl_easy_init();
	if(curl) {
		std::string filename = tmpDir + file;
		fp = fopen(filename.c_str(), "wb");

		curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, (void *)this);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_func);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK) {
			#ifdef DEBUG
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				std::cout << std::endl;
			#endif
		}
		fclose(fp);

		curl_easy_cleanup(curl);
	}
	else
		std::cout << "Curl failed to load" << std::endl;

	if (res == CURLE_OK)
		return 0;

	return -3;
}
