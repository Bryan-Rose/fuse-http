#include "util.h"

#include <ctime>

std::string ISO8601(tm* t) {
  char buf[64];
  strftime(buf, sizeof(buf), "%FT%TZ", t);

  return std::string(buf);
}
