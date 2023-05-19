#pragma once

#include <string>
#include <vector>
#include "ltypes.h"

std::string ISO8601(tm* t);

std::string dir_up(std::string s, std::string* endPath);

HFile* findFileName(std::vector<HFile*>& v, std::string& name);
