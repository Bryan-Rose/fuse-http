#include <iostream>
#include <string>
#include <regex>
#include <ctime>
#include <iomanip>
#include <curl/curl.h>

#include "ltypes.h"


// Def
void parse_single(HFile* record, std::string url, std::string ts, std::string size);
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

typedef std::regex_token_iterator<const char*> Myiter;

void util_parseAll(std::string body, std::vector<HFile*>* records)
{
  // 0 - url
  // 1 - timestampe
  // 2 - bytes
  // <a href="Jeopardy.2023.05.09.720p.HDTV.x264.AC3.mkv">Jeopardy.2023.05.09.720p.HDTV.x264.AC3.mkv</a>         10-May-2023 06:11           410576237
  const std::regex pattern("<a href=\"(.+?)\">.+?</?a> +(.+?) {2,}(.+)");

  const int subgroups[] = { 1,2,3 };
  std::regex_token_iterator<std::string::iterator> c(body.begin(), body.end(), pattern, subgroups);
  std::regex_token_iterator<std::string::iterator> rend;
  std::string matches[3];

  while (c != rend) {
    for (int i = 0; i < 3 && c != rend; i++) {
      auto a = *c++;
      matches[i] = a.str();
    }

    auto hf = new HFile();
    parse_single(hf, matches[0], matches[1], matches[2]);
    records->push_back(hf);
  }
}


void parse_single(HFile* record, std::string url, std::string ts, std::string size) {
  record->SizeBytes = stol(size);
  record->Url = url;
  record->Filename = util_getFileName(url);
  record->IsFile = record->Filename.back() != '/';

  std::istringstream dStr(ts);
  tm* tmPtr = &((*record).Timestamp);
  // 13-May-2023 06:46
  dStr >> std::get_time(tmPtr, "%d-%b-%Y %R");
}

bool util_readProcess(char* cmd, std::string* output) {
  FILE* fp;
  char buf[1024]; // working buffer for reading strings

  if (!(fp = popen(cmd, "r")))
  {
    return false; // open pipe
  }

  while (fgets(buf, sizeof(buf), fp))
  {
    output->append(buf);
  }
  pclose(fp);

  return true;
}
