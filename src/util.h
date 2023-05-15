#pragma once

#include <iostream>
#include <string>
#include <regex>
#include <ctime>
#include <iomanip>
#include <curl/curl.h>

#include "ltypes.h"

/// @brief Parse all records into vector
/// @param body 
/// @param records 
void util_parseAll(std::string body, std::vector<HFile*>* records);


/// @brief Read stdout
/// @param cmd 
/// @param output 
/// @return True if success, False if not
bool util_readProcess(char* cmd, std::string* output);
