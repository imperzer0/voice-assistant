// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.

#include "config.h"
#include <cstring>


static std::list<std::pair<std::string, std::string>> config;

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
		
		config.push_back({ key_phrase_start, value_start });
	}
	
	fclose(fp);
	
	return false;
}

void print_config()
{
	for (const auto& kv : config)
		fprintf(stderr, "\"%s\"=\"%s\"\n", kv.first.c_str(), kv.second.c_str());
}


// Function to search for a pattern in a string using the Boyer-Moore algorithm
int find_substr(const std::string& text, const std::string& pattern)
{
	int n = text.length();
	int m = pattern.length();
	
	int badchar[256];
	
	for (int i = 0; i < 256; i++)
		badchar[i] = -1;
	
	for (int i = 0; i < m; i++)
		badchar[(int)pattern[i]] = i;
	
	int s = 0; // s is the shift of the pattern with respect to the text
	while (s <= (n - m))
	{
		int j = m - 1;
		
		while (j >= 0 && pattern[j] == text[s + j])
			j--;
		
		if (j < 0)
		{
			return s;
		}
		else
			s += std::max(1, j - badchar[text[s + j]]);
	}
	return -1;
}

std::list<std::pair<std::string, std::string>>::iterator match_command(const std::string& voice_command)
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
	
	for (auto action = config.begin(); action != config.end(); ++action)
		if (find_substr(normalized_command, action->first) >= 0)
			return action;
	
	return config.end();
}

std::list<std::pair<std::string, std::string>>::iterator match_command_failed()
{
	return config.end();
}