#include <iostream>
#include <string>
#include <regex>
#include <ctime>
#include <iomanip>
#include <curl/curl.h>

#include "ltypes.h"


// Def
void parse_single(HFile* record, std::smatch matchResult);
std::string util_getFileName(std::string url);
// Def


std::string util_getFileName(std::string url) {
  CURL* curl;
  int decodeLength;
  char* decodeStr;

  curl = curl_easy_init();
  decodeStr = curl_easy_unescape(curl, url.c_str(), url.length(), &decodeLength);
  std::string r(decodeStr, decodeLength);
  return r;
}


void util_parseAll(std::string body, std::vector<HFile*>* records)
{
  // 0 - url
  // 1 - timestampe
  // 2 - bytes
  // <a href="Jeopardy.2023.05.09.720p.HDTV.x264.AC3.mkv">Jeopardy.2023.05.09.720p.HDTV.x264.AC3.mkv</a>         10-May-2023 06:11           410576237
  const std::regex pattern("<a href=\"(.+?)\">.+?</?a> +(.+?) {2,}(.+)");
  
  std::smatch matchResult;

  while (std::regex_search(body, matchResult, pattern)) {
    auto hf = new HFile();
    parse_single(hf, matchResult);
    records->push_back(hf);
  }
}

void parse_single(HFile* record, std::smatch matchResult) {
  record->SizeBytes = stoi(matchResult[3].str());
  const auto url = matchResult[1].str();
  record->Url = url;
  record->Filename = util_getFileName(url);
  record->IsFile = record->Filename.back() == '/';

  std::istringstream dStr(matchResult[2].str());
  tm* tmPtr = &((*record).Timestamp);
  // 13-May-2023 06:46
  dStr >> std::get_time(tmPtr, "%d-%b-%Y %R");
}

// void util_parse(std::string body, HFile* record)
// {
//   std::smatch matchResult;

//   // 0 - url
//   // 1 - timestampe
//   // 2 - bytes
//   // <a href="Jeopardy.2023.05.09.720p.HDTV.x264.AC3.mkv">Jeopardy.2023.05.09.720p.HDTV.x264.AC3.mkv</a>         10-May-2023 06:11           410576237
//   const std::regex pattern("<a href=\"(.+?)\">.+?</?a> +(.+?) {2,}(.+)");
//   auto isMatch = std::regex_search(body, matchResult, pattern);
//   if (isMatch == false || matchResult.ready() == false)
//   {
//     return;
//   }

//   record->SizeBytes = stoi(matchResult[2].str());
//   const auto url = matchResult[0].str();
//   record->Url = url;
//   record->Filename = util_getFileName(url);
//   record->IsFile = record->Filename.back() == '/';

//   std::istringstream dStr(matchResult[1].str());
//   tm* tmPtr = &((*record).Timestamp);
//   // 13-May-2023 06:46
//   dStr >> std::get_time(tmPtr, "%d-%b-%Y %R");
// }


bool util_readProcess(char* cmd, std::string* output) {
  FILE* fp;
  char buf[1024]; // working buffer for reading strings

  if (!(fp = popen(cmd, "r")))
  {
    return false; // open pipe
  }

  while (fgets(buf, sizeof(buf), fp))
  {
    output->append(buf, sizeof(buf));
  }
  pclose(fp);

  return true;
}
