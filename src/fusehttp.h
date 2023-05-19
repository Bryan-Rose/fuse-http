#if FALSE

// Hackaday filesystem class definition

#pragma once

#include "Fuse.h"
#include <fuse3/fuse.h>
#include "Fuse-impl.h"

class FHTTP : public Fusepp::Fuse<FHTTP>
{
public:
  FHTTP() {}

  ~FHTTP() {}

  static int getattr(const char*, struct stat*, struct fuse_file_info*);

  static int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags);

  static int open(const char* path, struct fuse_file_info* fi);

  static int read(const char* path, char* buf, std::size_t size, off_t offset, struct fuse_file_info* fi);

  static bool userinit(void);

  static int pathfind(const char* path);
};

#endif