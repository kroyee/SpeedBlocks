#ifndef PATCHCHECK_H
#define PATCHCHECK_H

#include <string>
#include <json.hpp>

struct PatchCheck {
	std::string exec(const std::string& cmd);
	void check(int version);
	int download_file(const std::string& file);
	bool check_md5(const std::string& file, const std::string& md5);
	void apply();

	std::string sendPost(const std::string& _request, const std::string& body);

	nlohmann::json j1;

	int status=0;
	int files_total=0;
	int files_downloaded=0;
};

#endif