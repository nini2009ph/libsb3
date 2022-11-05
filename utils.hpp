#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <chrono>
#include <regex>

#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <zip.h>

#define JSON_NULL {}

namespace libsb3 
{
	namespace utils
	{
		std::string unzip(std::string zip_path, std::string file_name);
		void writezip(std::string zip_path, std::string file_name, std::string buffer);
		size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data);

		class scratchsession
		{
		public:
			std::string username;
			std::string password;
			std::string session_id;
			std::string x_token;
			std::string csrf_token;

			scratchsession(std::string username, std::string password);
		};

		std::string generate_uuid();
	}
}

#endif