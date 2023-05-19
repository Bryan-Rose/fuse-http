
#include "fuseBinder.h"

#include <sys/stat.h>
#include <iostream>
#include <stdio.h> // using popen
#include <string>
#include <array>
#include <regex>
#include <ctime>
#include <iomanip>

#include "ltypes.h"
#include "fusehttp.h"
#include "util.h"
// include in one .cpp file
#include "Fuse-impl.h"
#include "remoteWrapper.h"

static const std::string root_path = "/";

std::string FuseBinder::mountPoint;
RemoteWrapper FuseBinder::rw;
FCache FuseBinder::cache;

void FuseBinder::Set(const std::string mountPoint, const RemoteWrapper& rw)
{
    FuseBinder::mountPoint = mountPoint;
    FuseBinder::rw = rw;
}

// Fuse wants to stat our files
int FuseBinder::getattr(const char* path, struct stat* stbuf, struct fuse_file_info*)
{
    std::string sPath{ path };

    if (path == root_path)
    {
        stbuf->st_mode = S_IFDIR | 0755; // our root directory
        stbuf->st_nlink = 2;
        return 0;
    }


    auto cacheEntry = FuseBinder::cache.GetFile(sPath);
    if (cacheEntry == nullptr) {
        cacheEntry = new FileInfoCacheEntry{};
        std::vector<HFile*> files;
        std::string endPath{ "" };
        std::string parentDir = dir_up(path, &endPath);
        FuseBinder::rw.PullAndRead(parentDir, files);

        auto foundFile = findFileName(files, endPath);
        if (foundFile == nullptr) {
            return -ENOENT; // not found
        }

        cacheEntry->FullPath = path;
        cacheEntry->Name = foundFile->Filename;
        cacheEntry->SizeBytes = foundFile->SizeBytes;
        cacheEntry->Timestamp = mktime(&(foundFile->Timestamp));

        FuseBinder::cache.Store(cacheEntry);
    }

    memset(stbuf, 0, sizeof(struct stat));

    stbuf->st_mode = S_IFREG | 0444;  // read only
    stbuf->st_nlink = 1;             // no links
    stbuf->st_size = cacheEntry->SizeBytes;

    return 0;
}

/// @brief Fuse wants to read a directory
/// @param path 
/// @param buf 
/// @param filler 
/// @param ot
/// @param ffi
/// @param frf
/// @return 
int FuseBinder::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t ot, struct fuse_file_info* ffi, enum fuse_readdir_flags frf)
{
    std::string sPath(path);
    std::vector<HFile*> entries;
    struct stat st;

    auto cacheEntry = FuseBinder::cache.GetFolder(sPath);
    if (cacheEntry == nullptr) {
        cacheEntry = new FolderInfoCacheEntry{};

        FuseBinder::rw.PullAndRead(sPath, entries);
        cacheEntry->FullPath = path;
        // . and ..
        cacheEntry->Entries.push_back(new EntryInfo{ .Name {"."}, .EntryType { EntryTypeEnum::Folder }, .SizeBytes = {-1} });
        cacheEntry->Entries.push_back(new EntryInfo{ .Name {".."}, .EntryType { EntryTypeEnum::Folder }, .SizeBytes = {-1} });

        // plus all of our "files"
        for (const auto e : entries) {
            auto eType = e->IsFile ? EntryTypeEnum::File : EntryTypeEnum::Folder;
            cacheEntry->Entries.push_back(new EntryInfo{ .Name {e->Filename}, .EntryType { eType }, .SizeBytes = { e->SizeBytes } });
            delete e;
        }

        FuseBinder::cache.Store(cacheEntry);
    }

    for (const auto e : cacheEntry->Entries) {
        memset(&st, 0, sizeof(st));
        if (e->EntryType == EntryTypeEnum::File) {
            st.st_size = e->SizeBytes;
            st.st_mode = S_IFREG;
        }
        else {
            st.st_mode = S_IFDIR;
        }

        if (filler(buf, e->Name.c_str(), &st, 0, FUSE_FILL_DIR_PLUS) == 1) {
            break;
        }
    }

    return 0;
}

// // Fuse wants to open a file
// int FuseBinder::open(const char* path, struct fuse_file_info* fi)
// {
//     std::string fn = path + 1; // skip leading /
//     int n;
//     if (fn.find_first_of('/') != std::string::npos)
//         return -ENOENT;                     // no sub dirs
//     if ((n = pathfind(fn.c_str())) == -1) // find fd
//         return -ENOENT;

//     if ((fi->flags & 3) != O_RDONLY) // must be read only
//         return -EACCES;
//     // fi->fh is file handle
//     fi->fh = n;
//     if (topic[n - 1].content.empty()) // if we have no content in cache, try to read now
//     {
//         topic[n - 1].content = readurl(topic[n - 1].url);
//     }
//     return 0;
// }

// // Fuse wants to read something
// int FuseBinder::read(const char* path, char* buf, std::size_t size, off_t offset, struct fuse_file_info* fi)
// {
//     std::size_t len;
//     path = path;
//     len = topic[fi->fh - 1].content.length(); // we get the length
//     if ((std::size_t)offset < len)            // if the offset isn't past the end...
//     {
//         if (offset + size > len)
//             size = len - offset;
//         memcpy(buf, topic[fi->fh - 1].content.c_str() + offset, size); // get the data over to the buffer
//     }
//     else
//         size = 0; // no data here
//     return size;
// }
