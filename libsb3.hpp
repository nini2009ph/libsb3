#ifndef LIBSB3_HPP
#define LIBSB3_HPP

#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>
#include <curl/curl.h>

#include "utils.hpp"
#include "blocks.hpp"

#define STAGE "Stage"

#define ATTACHED 0
#define TOPLEVEL 1

#define LATEST_BLOCK(SB3) SB3.get_block(0)

#define AUTO_BLOCK(SB3, TYPE, SPRITE, BLOCK) SB3.create_block(TYPE, SPRITE); LATEST_BLOCK(SB3).opcode = BLOCK
#define AUTO_BLOCK_IN(SB3, TYPE, SPRITE, BLOCK) SB3.create_block(TYPE, SPRITE); LATEST_BLOCK(SB3).opcode = BLOCK; LATEST_BLOCK(SB3).inputs = BLOCK##_IN
#define AUTO_PARENT(SB3, OFFSET) LATEST_BLOCK(SB3).parent = SB3.get_block(OFFSET).uuid

namespace libsb3
{
	class sb3
	{
	public:
		class block
		{
		public:		
			std::string uuid;
			std::string opcode = "";
			std::string next = "";
			std::string parent = "";
			nlohmann::json inputs = nlohmann::json::object();
			nlohmann::json fields = nlohmann::json::object();
			bool shadow = false;
			bool top_level = true;
			int x = 0;
			int y = 0;
			
			int block_type;
			sb3& _sb3;
			std::string sprite;

			void place();
			nlohmann::json& get();

			block(int block_type, sb3& _sb3, std::string sprite);
		};
		
		std::string path = "";
		int project_id = 0;
		std::string username;
		std::string password;
		
		nlohmann::json json = nlohmann::json::object();

		std::vector<block> blocks; 
		
		bool save();

		nlohmann::json& get_sprite(std::string name);
		nlohmann::json& get_stage();

		nlohmann::json& get_variable(std::string name, std::string sprite);
		std::string get_variable_id(std::string name, std::string sprite);
		void create_variable(std::string name, std::string value, std::string sprite);
		void delete_variable(std::string name, std::string sprite);

		nlohmann::json& get_block(std::string uuid, std::string sprite);
		block& get_block(int offset);
		void create_block(int block_type, std::string sprite);
		void place_blocks();

		sb3(std::string path);
		sb3(int project_id, std::string username, std::string password);
	};
}

#endif