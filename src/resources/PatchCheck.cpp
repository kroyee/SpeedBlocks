#include "PatchCheck.h"
#include <SFML/Network.hpp>
#include <fstream>
using json = nlohmann::json;
using std::to_string;

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
		if (!download_file(it.key())) {
			status=-3;
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

bool PatchCheck::download_file(const std::string& file) {
	sf::Http Http("http://speedblocks.se");
	
	std::string sURI = "/update/" + file;

	sf::Http::Request Request(sURI, sf::Http::Request::Get);

    sf::Http::Response Page = Http.sendRequest(Request);

    if (Page.getStatus() != sf::Http::Response::Ok) {
    	std::cout << Page.getStatus() << std::endl;
    	std::cout << "Error downloading file: " << file << std::endl;
    	return false;
    }

    std::string filename = resourcePath() + "tmp/" + file.substr(file.find('/')+1);
    std::ofstream ofile(filename, std::ios::binary);
	if (!ofile.is_open()) {
		std::cout << "Error saving file: " << file << std::endl;
		return false;
	}

    ofile.write((char*)Page.getBody().c_str(), Page.getBody().size());
	ofile.close();
	return true;
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
			copyto = copyto + "\\" + filename;
			std::string cmd = "move /y " + copyfrom + " " + copyto;
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