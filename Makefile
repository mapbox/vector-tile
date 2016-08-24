CC := $(CC)
CXX := $(CXX)
CXXFLAGS := $(CXXFLAGS) -Imason_packages/.link/include/ -Iinclude/mapbox/vector_tile -std=c++11
RELEASE_FLAGS := -O3 -DNDEBUG
WARNING_FLAGS := -Wall -Wextra -pedantic -Werror -Wsign-compare -Wfloat-equal -Wfloat-conversion -Wshadow -Wno-unsequenced
DEBUG_FLAGS := -g -O0 -DDEBUG -fno-inline-functions -fno-omit-frame-pointer

export BUILDTYPE ?= Release

ifeq ($(BUILDTYPE),Release)
	FINAL_FLAGS := -g $(WARNING_FLAGS) $(RELEASE_FLAGS)
else
	FINAL_FLAGS := -g $(WARNING_FLAGS) $(DEBUG_FLAGS)
endif

default: test

./.mason/mason:
	git clone https://github.com/mapbox/mason.git .mason
	cd .mason && git checkout 95ab9ca

mason_packages/headers/protozero: .mason/mason
	.mason/mason install protozero 1.4.0 && .mason/mason link protozero 1.4.0

mason_packages/headers/geometry: .mason/mason
	.mason/mason install geometry 0.8.0 && .mason/mason link geometry 0.8.0

mason_packages/headers/variant: .mason/mason
	.mason/mason install variant 1.1.1 && .mason/mason link variant 1.1.1

deps: mason_packages/headers/geometry mason_packages/headers/variant mason_packages/headers/protozero

build/$(BUILDTYPE)/test: test/unit/* include/mapbox/vector_tile/* Makefile
	mkdir -p build/$(BUILDTYPE)/
	$(CXX) $(FINAL_FLAGS) test/unit/*.cpp -Itest/include $(CXXFLAGS) -o build/$(BUILDTYPE)/test

test/mvt-fixtures:
	git submodule update --init

test: deps build/$(BUILDTYPE)/test test/mvt-fixtures
	./build/$(BUILDTYPE)/test

SOURCES = $(include/mapbox/vector_tile.hpp)
HEADERS = $(wildcard include/mapbox/vector_tile/*.hpp)
COMMON_DOC_FLAGS = --report --output docs $(HEADERS)

clean:
	rm -f build/
	rm -rf mason_packages

cldoc:
	pip install cldoc --user

docs: cldoc
	cldoc generate $(CXXFLAGS) -- $(COMMON_DOC_FLAGS)