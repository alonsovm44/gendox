CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -Wall -Wextra -I./tree-sitter/lib/include -O2
CFLAGS =  -I./tree-sitter/lib/include -O2 -DTS_NO_UTF8PROC

# Tree-sitter core
TS_DIR = tree-sitter
TS_SRC = $(TS_DIR)/lib/src/lib.c
TS_OBJ = tree-sitter-lib.o

# Language parsers
LANG_DIRS = \
    tree-sitter-c \
    tree-sitter-cpp \
    tree-sitter-python \
    tree-sitter-javascript \
    tree-sitter-typescript/typescript \
    tree-sitter-go \
    tree-sitter-rust

# Automatically find all C and C++ source files for the parsers and scanners
LANG_C_SRCS = $(foreach dir,$(LANG_DIRS),$(wildcard $(dir)/src/parser.c) $(wildcard $(dir)/src/scanner.c))
LANG_CXX_SRCS = $(foreach dir,$(LANG_DIRS),$(wildcard $(dir)/src/scanner.cc))

LANG_C_OBJS = $(patsubst %.c,%.o,$(LANG_C_SRCS))
LANG_CXX_OBJS = $(patsubst %.cc,%.o,$(LANG_CXX_SRCS))

DOCGEN_SRCS = src/main.cpp
DOCGEN_OBJS = $(DOCGEN_SRCS:.cpp=.o)

TARGET = docgen

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(DOCGEN_OBJS) $(TS_OBJ) $(LANG_C_OBJS) $(LANG_CXX_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TS_OBJ): $(TS_SRC)
	$(CC) $(CFLAGS) -I$(TS_DIR)/lib/src -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -I$(<D) -c $< -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -I$(<D) -c $< -o $@

clean:
	rm -f $(DOCGEN_OBJS) $(TS_OBJ) $(LANG_C_OBJS) $(LANG_CXX_OBJS) $(TARGET)