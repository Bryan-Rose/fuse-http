#pragma once

#include <string>
#include <vector>

#include "ltypes.h"

class RemoteWrapper {
public:
    RemoteWrapper(std::string user, std::string password, std::string urlBase);
    ~RemoteWrapper();

    void PullAndRead(std::string subDir, std::vector<HFile*>* files);

private:
    std::string user;
    std::string password;
    std::string urlBase;
    std::string build_command(std::string localPath);

};
