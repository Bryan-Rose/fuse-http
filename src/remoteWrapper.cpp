#include "remoteWrapper.h"

#include <sstream>
#include <iostream>
#include <string>
#include <regex>
#include <ctime>
#include <iomanip>
#include <curl/curl.h>

#include "ltypes.h"


// Def
void parseSingle(HFile* record, std::string url, std::string ts, std::string size);
void parseAll(std::string body, std::vector<HFile*>& records);
std::string util_getFileName(std::string url);
bool readProcess(const char* cmd, std::string* output);
std::string pathJoin(std::vector<std::string> segments);
// Def

RemoteWrapper::RemoteWrapper(std::string user, std::string password, std::string urlBase) {
    this->user = user;
    this->password = password;
    this->urlBase = urlBase;
}

RemoteWrapper::~RemoteWrapper() { }

void RemoteWrapper::PullAndRead(std::string subDir, std::vector<HFile*>& files) {
    auto cmd = this->build_command(subDir);
    std::string html;
    readProcess(cmd.c_str(), &html);
    parseAll(html, files);

    for (auto f : files) {
        f->FullUrl = pathJoin(std::vector<std::string>{
            this->urlBase,
                subDir,
                f->EncodedFileName
        });
    }
}


std::string RemoteWrapper::build_command(std::string localPath)
{
    std::stringstream ss;
    // "curl --silent --user \"{0}:{1}\" \"{2}{3}\" | grep -P \"<a\"";
    ss << R"(curl --silent --user ")" << this->user << ":" << this->password << R"(" ")" << this->urlBase << localPath << R"(" | grep -P "<a")";

    return ss.str();
}


/**************************************************************************/

bool readProcess(const char* cmd, std::string* output) {
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

std::string util_getFileName(std::string url) {
    CURL* curl;
    int decodeLength;
    char* decodeStr;

    curl = curl_easy_init();
    decodeStr = curl_easy_unescape(curl, url.c_str(), url.length(), &decodeLength);
    std::string r(decodeStr, decodeLength);
    return r;
}

void parseAll(std::string body, std::vector<HFile*>& records)
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
        parseSingle(hf, matches[0], matches[1], matches[2]);
        records.push_back(hf);
    }
}

void parseSingle(HFile* record, std::string url, std::string ts, std::string size) {
    record->EncodedFileName = url;
    record->Filename = util_getFileName(url);
    record->IsFile = record->Filename.back() != '/';
    record->SizeBytes = (record->IsFile ? stol(size) : -1);

    std::istringstream dStr(ts);
    tm* tmPtr = &((*record).Timestamp);
    // 13-May-2023 06:46
    dStr >> std::get_time(tmPtr, "%d-%b-%Y %R");
}

std::string pathJoin(std::vector<std::string> segments) {
    std::string result(segments[0]);

    for (auto s : segments) {
        int subStart = 0;
        int subStop = s.length() - 1;
        result.append(s.substr(subStart, subStop));
    }

    return result;
}


/**************************************************************************/
