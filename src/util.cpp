#include "util.h"

#include <ctime>
#include <vector>

std::string ISO8601(tm* t) {
  char buf[64];
  strftime(buf, sizeof(buf), "%FT%TZ", t);

  return std::string(buf);
}

std::string dir_up(std::string s, std::string* endPath) {
  if (s.length() == 1) {
    return s;
  }

  auto index = s.find_last_of('/');
  if (index == std::string::npos) {
    return s;
  }

  auto path = s.substr(0, index);
  auto file = s.substr(index + 1);
  if (endPath != nullptr) {
    endPath->swap(file);
  }
  return path;
}

HFile* findFileName(std::vector<HFile*>& v, std::string& name)
{
  for (const auto e : v) {
    if (e->Filename.compare(name) == 0) {
      return e;
    }
  }
  return nullptr;
}

