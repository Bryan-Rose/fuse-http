# Makefile for a standard C++ project using gcc on Linux

# Compiler and compiler flags
CXX = gcc
CXXFLAGS = -std=c++17 -Wall -Wextra #-pedantic 
LDFLAGS = $(shell pkg-config fuse3 --libs) -lstdc++
INCLUDES = -Iinclude $(shell pkg-config fuse3 --cflags)
 
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
	$(CXX) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(INCLUDES) $^

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) -o $@ $(CXXFLAGS) $(INCLUDES) -c $<

# Clean rule
clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
