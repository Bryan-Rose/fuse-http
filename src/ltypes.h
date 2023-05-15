#pragma once

#include <string>
#include <time.h>

struct HFile
{
    std::string Url;
    std::string Filename;
    tm Timestamp;
    long SizeBytes;
    bool IsFile;
};