#ifndef PATCHCHECK_H
#define PATCHCHECK_H

#include <string>
#include <unordered_map>

struct PatchCheck {
	PatchCheck();
	std::string exec(const std::string& cmd);
	void check(int version);
	int download_file(const std::string& file);
	bool check_md5(const std::string& file, const std::string& md5);
	int apply();
	void parseJson(const std::string& jsonString);
	bool cancelCheck();

	std::string sendPost(const std::string& _request, const std::string& body);

	std::unordered_map<std::string, std::string> j1;

	int status=0;
	int files_total=0;
	int files_downloaded=0;

	int64_t filesize=0, downloaded=0;

	std::string tmpDir;
	std::string changelog;

	bool quit=false;
};

#endif
