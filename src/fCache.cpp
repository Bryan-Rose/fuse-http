#include "fCache.h"

FCache::FCache()
{
}

FCache::~FCache()
{
    for (auto e : this->folderCache) {
        delete e.second;
    }
}

void FCache::Store(FolderInfoCacheEntry* e)
{
    // expire in 60 seconds
    e->Expires = time(NULL) + 60;
    this->folderCache[e->FullPath] = e;
}

void FCache::Store(FileInfoCacheEntry* e)
{
    // expire in 60 seconds
    e->Expires = time(NULL) + 60;
    this->fileCache[e->FullPath] = e;
}

FolderInfoCacheEntry* FCache::GetFolder(const std::string& fullPath)
{
    if (this->folderCache.contains(fullPath) == false) {
        return nullptr;
    }

    auto entry = this->folderCache[fullPath];
    if (entry->Expires >= time(NULL)) {

        return nullptr;
    }

    return entry;
}

FileInfoCacheEntry* FCache::GetFile(const std::string& fullPath)
{
    if (this->fileCache.contains(fullPath) == false) {
        return nullptr;
    }

    auto entry = this->fileCache[fullPath];
    if (entry->Expires >= time(NULL)) {

        return nullptr;
    }

    return entry;
}
