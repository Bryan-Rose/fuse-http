#pragma once

#include <string>
#include <time.h>

struct HFile
{
    std::string FullUrl;
    std::string EncodedFileName;
    std::string Filename;
    tm Timestamp;
    long SizeBytes;
    bool IsFile;
};