// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.

#include "config.h"
#include <cstring>


static std::unordered_map<std::string, std::string> config;

char* APIToken = nullptr;

bool parse_config(const char* config_file)
{
	// Open the configuration file
	FILE* fp = fopen(config_file, "r");
	if (fp == nullptr)
	{
		fprintf(stderr, "Error: Unable to open configuration file [file://%s].\n", config_file);
		return true;
	}
	
	char* save_ptr = nullptr;
	char line[256];
	
	// Check first line
	if (fgets(line, sizeof line, fp))
	{
		// Split the line into the key phrase and value
		char* key = strtok_r(line, "=", &save_ptr);
		char* value = strtok_r(nullptr, "\n", &save_ptr);
		
		if (strcmp(key, "token") != 0)
		{
			fprintf(stderr, "Error: No token field in config [file://%s].\n", config_file);
			fprintf(stderr, "First field should be\n  token:\"<rev.ai token>\"\n\n");
			return true;
		}
		
		// Check if the value is surrounded by double quotes
		char* value_start = value;
		if (*value_start == '"')
		{
			++value_start;
			auto* value_end = strrchr(value, '"');
			*value_end = '\0';
		}
		
		APIToken = strdup(value_start);
	}
	
	// Read the rest of the file line by line
	for (int l = 0; fgets(line, sizeof line, fp); ++l)
	{
		// Split the line into the key phrase and value
		char* key_phrase = strtok_r(line, "=", &save_ptr);
		char* value = strtok_r(nullptr, "\n", &save_ptr);
		
		// Check if the key phrase is surrounded by double quotes
		char* key_phrase_start = key_phrase;
		if (*key_phrase_start == '"')
		{
			++key_phrase_start;
			auto* key_phrase_end = key_phrase + strlen(key_phrase) - 1;
			if (*key_phrase_end != '"')
			{
				fprintf(stderr, "Error on line %d: Invalid config [file://%s], no '\"' in the end of key.\n", l, config_file);
				return true;
			}
			*key_phrase_end = '\0';
		}
		
		// Check if the value is surrounded by double quotes
		char* value_start = value;
		if (*value_start == '"')
		{
			++value_start;
			auto* value_end = value + strlen(value) - 1;
			if (*value_end != '"')
			{
				fprintf(stderr, "Error on line %d: Invalid config [file://%s], no '\"' in the end of value.\n", l, config_file);
				return true;
			}
			*value_end = '\0';
		}
		
		config[key_phrase_start] = value_start;
	}
	
	fclose(fp);
	
	return false;
}

void print_config()
{
	for (const auto& kv : config)
		fprintf(stderr, "\"%s\"=\"%s\"\n", kv.first.c_str(), kv.second.c_str());
}

std::unordered_map<std::string, std::string>::iterator match_command(const std::string& voice_command)
{
	std::string normalized_command;
	char prev = ' ';
	bool lt = false;
	for (auto& c : voice_command)
	{
		if (c == '<')
		{
			lt = true;
			continue;
		}
		if (c == '>')
		{
			lt = false;
			continue;
		}
		if (lt) continue;
		
		if (c == ' ' && prev != ' ' || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9')
		{
			normalized_command.push_back(std::tolower(c));
		}
		prev = c;
	}
	
	while (normalized_command.ends_with(' ')) normalized_command.pop_back();
	
	return config.find(normalized_command);
}

std::unordered_map<std::string, std::string>::iterator match_command_failed()
{
	return config.end();
}