#include "utils.hpp"

namespace libsb3
{
    namespace utils
    {
        std::string unzip(std::string zip_path, std::string file_name)
        {
            int error;
            zip* archive = zip_open(zip_path.c_str(), 0, &error);
            struct zip_stat stat;
            zip_stat_init(&stat);
            zip_stat(archive, file_name.c_str(), 0, &stat);
            char* contents = new char[stat.size];
            zip_file* file = zip_fopen(archive, file_name.c_str(), 0);
            zip_fread(file, contents, stat.size);
            zip_fclose(file);
            zip_close(archive);
            std::string output = contents;
            output.erase(output.length() - (output.length() - stat.size));
            return output;
        }

        void writezip(std::string zip_path, std::string file_name, std::string buffer)
        {
            int error;
            zip* archive = zip_open(zip_path.c_str(), 0, &error);
            struct zip_stat stat;
            zip_stat_init(&stat);
            zip_stat(archive, file_name.c_str(), 0, &stat);
            zip_delete(archive, zip_stat_index(archive, 0, 0, &stat));
            zip_source_t* source;
            source = zip_source_buffer(archive, buffer.c_str(), buffer.length(), 0);
            zip_file_add(archive, file_name.c_str(), source, ZIP_FL_ENC_UTF_8);
            zip_close(archive);
            zip_source_free(source);
        }

        size_t write_callback(char* ptr, size_t size, size_t nmemb, std::string* data) {
            data->append(ptr, size * nmemb);
            return size * nmemb;
        }

		scratchsession::scratchsession(std::string username, std::string password)
			: username(username)
			, password(password)
		{
			CURL* curl = curl_easy_init();
			curl_easy_setopt(curl, CURLOPT_URL, "https://scratch.mit.edu/login/");
			struct curl_slist* login_headers = NULL;
			login_headers = curl_slist_append(login_headers, "x-csrftoken: a");
			login_headers = curl_slist_append(login_headers, "x-requested-with: XMLHttpRequest");
			login_headers = curl_slist_append(login_headers, "Cookie: scratchcsrftoken=a;scratchlanguage=en;");
			login_headers = curl_slist_append(login_headers, "referer: https://scratch.mit.edu");
			login_headers = curl_slist_append(login_headers, "user-agent: a");
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, login_headers);
			std::string login_postfield = "{ \"username\": \"" + username + "\", \"password\": \"" + password + "\" }";
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, login_postfield.c_str());
			std::string login_response_body;
			std::string login_response_headers;
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &login_response_body);
			curl_easy_setopt(curl, CURLOPT_WRITEHEADER, utils::write_callback);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &login_response_headers);
			curl_easy_perform(curl);
			curl_slist_free_all(login_headers);

			std::smatch session_id_smatch;
			std::regex_search(login_response_headers, session_id_smatch, std::regex("\"(.*)\""));
			session_id = session_id_smatch[0];
			x_token = nlohmann::json::parse(login_response_body)[0]["token"].get<std::string>();

			curl_easy_reset(curl);

			curl_easy_setopt(curl, CURLOPT_URL, "https://scratch.mit.edu/csrf_token/");
			std::string csrf_token_response_headers;
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEHEADER, utils::write_callback);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &csrf_token_response_headers);
			curl_easy_perform(curl);

			std::smatch csrf_token_smatch;
			std::regex_search(csrf_token_response_headers, csrf_token_smatch, std::regex("scratchcsrftoken=(.*?);"));
			csrf_token = csrf_token_smatch[1];

			curl_easy_cleanup(curl);
		}

		std::string generate_uuid()
		{
			std::string uuid;
			for (int i = 0; i < 20; i++) 
			{
				std::srand(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + rand());
				uuid += std::to_string(std::rand() % 10);
			}	
			return uuid;
		}
    }
}