// Hackaday RSS filesystem class implementation

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

using namespace std;

static const string root_path = "/";

// Maximum number of topics
#define MAXTOPIC ((unsigned long)128)
static unsigned topics = 0; // number of topics we've seen

// Record for RSS item
struct a_topic
{
  std::string title;   // title
  std::string url;     // URL
  std::string content; // content
};

// Where the topics live
array<a_topic, MAXTOPIC> topic;

// The curl line to read our feed
static char cmd[] = "curl --silent --user \"user6637:k3CC59Z6B0HXOmf\" \"https://user6637.emerald.usbx.me/files/tv/\" | grep -P \"<a\"";

// Trim RSS lines to kill leading spaces and the last </...> part
// We are assuming the format will not have multiple things on one line
// Although that isn't a great assumption
static std::string trimrss(const std::string& str, const std::string& white1 = " \t\r\n", const std::string& white2 = "<")
{
  std::size_t first, last;
  first = str.find_first_not_of(white1);
  if (first == std::string::npos)
    return "";
  last = str.find_last_of(white2);
  if (last == std::string::npos)
    last = str.length() - 1;
  else
    last--; // untested if there is no <...> at the end
  return str.substr(first, last - first + 1);
}

// Find a path and return a file descriptor (can't be zero so we add 1 to it)
int FHTTP::pathfind(const char* path)
{
  std::string s = path;
  if (s.find_first_of('/') != std::string::npos)
  {
    // no subdirectories
    return -1;
  }
  for (unsigned int i = 0; i < topics; i++)
    if (s == topic[i].title)
      return i + 1;
  return -1;
}

// Read the content or any URL for that matter
static std::string readurl(std::string& url)
{
  FILE* fp;
  char buf[1024]; // working buffer for reading strings
  std::string result, curl = "2>/dev/null curl '";
  curl += url;
  curl += "'";
  if (!(fp = popen(curl.c_str(), "r")))
    return "";
  while (fgets(buf, sizeof(buf), fp))
  {
    result += buf;
    result += "\n";
  }
  pclose(fp);
  return result;
}

// Fuse wants to stat our files
int FHTTP::getattr(const char* path, struct stat* stbuf, struct fuse_file_info*)
{
  int res = 0, n;
  memset(stbuf, 0, sizeof(struct stat));
  if (path == root_path)
  {
    stbuf->st_mode = S_IFDIR | 0755; // our root directory
    stbuf->st_nlink = 2;
  }
  else if ((n = pathfind(path + 1)) > 0) // find path (skip leading /)
  {
    stbuf->st_mode = S_IFREG | 0444;  // read only
    stbuf->st_nlink = 1;              // no links
    if (topic[n - 1].content.empty()) // if empty, go ahead and fill content cache
    {
      topic[n - 1].content = readurl(topic[n - 1].url);
    }
    stbuf->st_size = topic[n - 1].content.length(); // now we know the length
  }
  else
  {
    res = -ENOENT; // not found
  }
  return res;
}

// Fuse wants to read a directory
int FHTTP::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t, struct fuse_file_info*, enum fuse_readdir_flags)
{
  if (path != root_path) // only root is valid
    return -ENOENT;
  // we always have . and ..
  filler(buf, ".", nullptr, 0, FUSE_FILL_DIR_PLUS);
  filler(buf, "..", nullptr, 0, FUSE_FILL_DIR_PLUS);
  // plus all of our "files"
  for (unsigned int i = 0; i < topics; i++)
    filler(buf, topic[i].title.c_str(), nullptr, 0, FUSE_FILL_DIR_PLUS);
  return 0;
}

// Fuse wants to open a file
int FHTTP::open(const char* path, struct fuse_file_info* fi)
{
  std::string fn = path + 1; // skip leading /
  int n;
  if (fn.find_first_of('/') != std::string::npos)
    return -ENOENT;                     // no sub dirs
  if ((n = pathfind(fn.c_str())) == -1) // find fd
    return -ENOENT;

  if ((fi->flags & 3) != O_RDONLY) // must be read only
    return -EACCES;
  // fi->fh is file handle
  fi->fh = n;
  if (topic[n - 1].content.empty()) // if we have no content in cache, try to read now
  {
    topic[n - 1].content = readurl(topic[n - 1].url);
  }
  return 0;
}

// Fuse wants to read something
int FHTTP::read(const char* path, char* buf, std::size_t size, off_t offset, struct fuse_file_info* fi)
{
  std::size_t len;
  path = path;
  len = topic[fi->fh - 1].content.length(); // we get the length
  if ((std::size_t)offset < len)            // if the offset isn't past the end...
  {
    if (offset + size > len)
      size = len - offset;
    memcpy(buf, topic[fi->fh - 1].content.c_str() + offset, size); // get the data over to the buffer
  }
  else
    size = 0; // no data here
  return size;
}


bool userTest()
{
  std::string fullResponse;
  std::vector<HFile*> hFiles;

  if (util_readProcess(cmd, &fullResponse) == false) {
    cout << "Process read failed" << endl;
    return false;
  }

  util_parseAll(fullResponse, &hFiles);
  for (auto x : hFiles) {
    cout << x->Filename << endl
      << x->Url << endl
      << x->IsFile << endl
      << x->Timestamp.tm_mday << endl
      << x->SizeBytes << endl;
  }

  return true;
}
