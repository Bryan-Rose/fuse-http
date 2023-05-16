# Makefile for a standard C++ project using gcc on Linux

# Compiler and compiler flags
CXX = gcc
CXXFLAGS = -std=c++17 -Wall -Wextra -g -D DEBUG
INCLUDES = -Iinclude $(shell pkgconf fuse3 --cflags)
LDFLAGS = $(shell pkgconf fuse3 --libs) $(shell pkgconf --libs libcurl) -lstdc++
 
# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Source files and object files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Target executable
TARGET = htfs

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(BINDIR)/$(TARGET) $(CXXFLAGS) $(LDFLAGS) $(INCLUDES) $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR) $(BINDIR)
	$(CXX) -o $@ $(CXXFLAGS) $(INCLUDES) -c $<

# Clean rule
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
