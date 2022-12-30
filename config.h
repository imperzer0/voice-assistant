// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


#ifndef ASSISTANT_CONFIG_H
#define ASSISTANT_CONFIG_H

#include <unordered_map>
#include <string>


extern char* APIToken;

extern bool parse_config(const char* config_file);

extern void print_config();

extern std::unordered_map<std::string, std::string>::iterator match_command(const std::string& voice_command);

extern std::unordered_map<std::string, std::string>::iterator match_command_failed();

#endif //ASSISTANT_CONFIG_H
