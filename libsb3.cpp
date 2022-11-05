#include "libsb3.hpp"

namespace libsb3
{	
	sb3::block::block(int block_type, sb3& _sb3, std::string sprite)
		: block_type(block_type)
		, _sb3(_sb3)
		, sprite(sprite)
		, uuid(utils::generate_uuid())
	{
	}

	void sb3::block::place()
	{
		if (block_type == ATTACHED)
		{
			top_level = false;
			_sb3.get_block(parent, sprite)["next"] = uuid;
		}
			
		get() = { {"opcode", opcode},
				  {"next", next},
				  {"parent", parent},
				  {"inputs", inputs},
				  {"fields", fields},
				  {"shadow", shadow},
				  {"topLevel", top_level} };

		if (next == "")
			get()["next"] = JSON_NULL;
		if (parent == "")
			get()["parent"] = JSON_NULL;
		
		if (block_type == TOPLEVEL)
		{
			get()["x"] = x;
			get()["y"] = y;
		}
	}

	nlohmann::json& sb3::block::get() 
	{
		return _sb3.get_sprite(sprite)["blocks"][uuid];
	}
	
	bool sb3::save()
	{
		if (path != "")
		{
			utils::writezip(path, "project.json", json.dump());
		}
		else 
		{
			CURL* curl = curl_easy_init();
			curl_easy_setopt(curl, CURLOPT_URL, ("https://projects.scratch.mit.edu/" + std::to_string(project_id)).c_str());
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
			utils::scratchsession session(username, password);
			struct curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "Content-Type: application/json");
			curl_easy_setopt(curl, CURLOPT_COOKIE, ("scratchcsrftoken=" + session.csrf_token + "; scratchsessionsid=" + session.session_id + ";").c_str());
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
			std::string json_string = json.dump();
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_string.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::write_callback);
			std::string response;
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
			curl_easy_perform(curl);
			long http_code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl);
			if (http_code != 200)
				return false;
		}
		return true;
	}

	nlohmann::json& sb3::get_sprite(std::string name) 
	{
		for (nlohmann::json& item : json["targets"])
		{
			if (item["name"].get<std::string>() == name)
				return item;
		}
		return *((nlohmann::json*)nullptr);
	}
	
	nlohmann::json& sb3::get_stage() 
	{
		return json["targets"][0];
	}

	nlohmann::json& sb3::get_variable(std::string name, std::string sprite)
	{
		for (nlohmann::json& item : get_sprite(sprite)["variables"])
		{
			if (item[0].get<std::string>() == name)
				return item;
		}
		return *((nlohmann::json*)nullptr);
	}

	std::string sb3::get_variable_id(std::string name, std::string sprite)
	{
		for (auto& item : get_sprite(sprite)["variables"].items())
		{
			if (item.value().get<nlohmann::json>()[0] == name)
				return item.key();
		}
		return "";
	}

	void sb3::create_variable(std::string name, std::string value, std::string sprite)
	{
		get_sprite(sprite)["variables"][libsb3::utils::generate_uuid()] = { name, value };
	}

	void sb3::delete_variable(std::string name, std::string sprite)
	{
		get_sprite(sprite)["variables"].erase(get_variable_id(name, sprite));
	}

	nlohmann::json& sb3::get_block(std::string uuid, std::string sprite) 
	{
		return get_sprite(sprite)["blocks"][uuid];
	}

	sb3::block& sb3::get_block(int offset)
	{
		return blocks[blocks.size() - offset - 1];
	}

	void sb3::create_block(int block_type, std::string sprite)
	{
		blocks.emplace_back(block(block_type, *this, sprite));
	}

	void sb3::place_blocks()
	{
		for (block& _block : blocks)
			_block.place();
	}
	
	sb3::sb3(std::string path)
		: path(path)
		, json(nlohmann::json::parse(utils::unzip(path, "project.json")))
	{
	}

	sb3::sb3(int project_id, std::string username, std::string password)
		: project_id(project_id)
		, username(username)
		, password(password)
	{
		CURL* curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, ("https://projects.scratch.mit.edu/" + std::to_string(project_id)).c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, utils::write_callback);
		std::string response;
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		json = nlohmann::json::parse(response);
	}
}