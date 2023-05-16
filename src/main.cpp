#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstdlib>

#include "fusehttp.h"
#include "remoteWrapper.h"
#include "ltypes.h"
#include "util.h"

struct Args {
  std::string user;
  std::string password;
  std::string urlBase;
  std::string mountPoint;
};

void print_help() {
  std::cout << "./htfs user password urlBase mountPoint" << std::endl;
}

int run_print(Args args) {
  RemoteWrapper rw(args.user, args.password, args.urlBase);
  std::cout << "Printing" << std::endl;
  std::vector<HFile*> files;
  rw.PullAndRead("", &files);
  for (const auto f : files) {
    std::cout << "Filename: " << f->Filename << std::endl
      << "Type: " << (f->IsFile ? "File" : "Folder") << std::endl
      << "TS: " << ISO8601(&f->Timestamp) << std::endl
      << "Size: " << f->SizeBytes << std::endl << std::endl;
  }

  return 0;
}

int run_mount(Args args) {
  std::cout << "Mounting" << std::endl;
  std::cout << "unsupported" << std::endl;
  // bool success = userTest();
  // return success ? 0 : 1;
}

int main(int argc, char* argv[]) {

  std::vector<std::string> sargs;

  for (int i = 0;i < argc; i++) {
    sargs.push_back(std::string(argv[i]));
  }

  if (sargs.size() == 1) {

    if (auto user = std::getenv("D_USERNAME")) {
      sargs.push_back(std::string(user));
    }
    if (auto password = std::getenv("D_PASSWORD")) {
      sargs.push_back(std::string(password));
    }
    if (auto url = std::getenv("D_URL")) {
      sargs.push_back(std::string(url));
    }
    if (auto mountPoint = std::getenv("D_MOUNTPOINT")) {
      sargs.push_back(std::string(mountPoint));
    }
  }



  if (sargs.size() == 5) {
    Args a{
      .user = sargs[1],
      .password = sargs[2],
      .urlBase = sargs[3],
      .mountPoint = sargs[4],
    };
    return run_mount(a);
  }
  else if (sargs.size() == 4) {
    Args a{
    .user = sargs[1],
      .password = sargs[2],
      .urlBase = sargs[3],
    };
    return run_print(a);
  }
  else {
    print_help();
  }


  //FHTTP fs;
  // if (fs.userinit() == false) {
  //   fprintf(stderr, "Can't fetch feed\n");
  //   return 99;
  // }

  // int status;
  // status = fs.run(argc, argv);

  // return status;

  //return 0;
}

