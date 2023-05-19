#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <ctime>


enum EntryTypeEnum {
    File = 1,
    Folder = 2
};

struct EntryInfo {
    std::string Name;
    EntryTypeEnum EntryType;
    long SizeBytes;
    long Timestamp;
};

struct FolderInfoCacheEntry {
    std::time_t Expires;
    std::string Name;
    std::string FullPath;
    std::vector<EntryInfo*> Entries;
};

struct FileInfoCacheEntry {
    std::time_t Expires;
    std::string FullPath;
    std::string Name;
    long SizeBytes;
    long Timestamp;
};

class FCache {
public:
    FCache();
    ~FCache();

    /// @brief 
    /// @param e 
    void Store(FolderInfoCacheEntry* e);
    
    /// @brief 
    /// @param e 
    void Store(FileInfoCacheEntry* e);

    /// @brief 
    /// @param fullPath 
    /// @return Pointer if it exists, otherwith nullptr
    FolderInfoCacheEntry* GetFolder(const std::string& fullPath);

    /// @brief 
    /// @param fullPath 
    /// @return 
    FileInfoCacheEntry* GetFile(const std::string& fullPath);

private:
    std::unordered_map<std::string, FolderInfoCacheEntry*> folderCache;
    std::unordered_map<std::string, FileInfoCacheEntry*> fileCache;
};
