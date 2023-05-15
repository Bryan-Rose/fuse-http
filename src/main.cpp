#include <stdio.h>

#include "fusehttp.h"

int main(int argc, char* argv[]) {
  bool success = userTest();
  return success ? 0 : 1;

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
