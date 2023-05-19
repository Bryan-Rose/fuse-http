#pragma once

#include "Fuse.h"

#include <string>
#include <fuse.h>
#include "Fuse-impl.h"

#include "remoteWrapper.h"
#include "fCache.h"



class FuseBinder : public Fusepp::Fuse<FuseBinder>
{
public:
    static void Set(const std::string mountPoint, const RemoteWrapper& rw);

    static int getattr(const char*, struct stat*, struct fuse_file_info*);

    static int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t ot, struct fuse_file_info* ffi, enum fuse_readdir_flags frf);


    // static int open(const char* path, struct fuse_file_info* fi);

    // static int read(const char* path, char* buf, std::size_t size, off_t offset, struct fuse_file_info* fi);

private:
    static std::string mountPoint;
    static RemoteWrapper rw;
    static FCache cache;
};
