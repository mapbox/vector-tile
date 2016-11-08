CC := $(CC)
CXX := $(CXX)
CXXFLAGS := $(CXXFLAGS) -Iinclude -std=c++14 -Wall
RELEASE_FLAGS := -O3 -DNDEBUG -fvisibility-inlines-hidden -fvisibility=hidden
DEBUG_FLAGS := -g -O0 -DDEBUG -fno-inline-functions -fno-omit-frame-pointer

export BUILDTYPE ?= Release

ifeq ($(BUILDTYPE),Release)
	FINAL_FLAGS := $(RELEASE_FLAGS)
else
	FINAL_FLAGS := $(DEBUG_FLAGS)
endif


default: decode

decode: decode.cpp
	$(CXX) decode.cpp $(FINAL_FLAGS) $(CXXFLAGS) -o decode


