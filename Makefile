# Copyright (c) 2022-2024 Dr. Colin Hirsch

.SUFFIXES:
.SECONDARY:

UNAME_S := $(shell uname -s)

# For Darwin (Mac OS X / macOS) we assume that the default compiler
# clang++ is used; when $(CXX) is some version of g++, then
# $(CXXSTD) has to be set to -std=c++17 (or newer) so
# that -stdlib=libc++ is not automatically added.

ifeq ($(CXXSTD),)
CXXSTD := -std=c++20
ifeq ($(UNAME_S),Darwin)
CXXSTD += -stdlib=libc++
endif
endif

CPPFLAGS ?= -pedantic
CXXFLAGS ?= -Wall -Wextra -Werror -O3

HEADERS := $(shell find src -name '*.hpp')
SOURCES := $(shell find src -name '*.cpp')
DEPENDS := $(SOURCES:src/%.cpp=build/dep/%.d)
BINARIES := $(SOURCES:src/%.cpp=build/bin/%)

.PHONY: all
 all: compile

.PHONY: compile
compile: $(BINARIES)

.PHONY: clean
clean:
	@rm -rf build/*
	@find . -name '*~' -delete

build/dep/%.d: src/%.cpp Makefile
	@mkdir -p $(@D)
	$(CXX) $(CXXSTD) $(CPPFLAGS) -MM -MQ $@ $< -o $@

build/bin%: src/%.cpp build/dep/%.d
	@mkdir -p $(@D)
	$(CXX) $(CXXSTD) $(CPPFLAGS) $(CXXFLAGS) $< $(LDFLAGS) -o $@

ifeq ($(findstring $(MAKECMDGOALS),clean),)
-include $(DEPENDS)
endif
