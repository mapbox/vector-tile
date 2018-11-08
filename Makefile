CC := $(CC)
CXX := $(CXX)
CXXFLAGS := $(CXXFLAGS) -isystem mason_packages/.link/include/ -Iinclude -std=c++14 -DPROTOZERO_STRICT_API=1
RELEASE_FLAGS := -O3 -DNDEBUG -flto -fvisibility-inlines-hidden -fvisibility=hidden
WARNING_FLAGS := -Wall -Wextra -pedantic -Werror -Wsign-compare -Wfloat-equal -Wconversion -Wshadow
CLANG_WARNING_FLAGS := -Wno-unsequenced -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-exit-time-destructors
ifneq (,$(findstring clang,$(CXX)))
    WARNING_FLAGS += $(CLANG_WARNING_FLAGS)
endif
DEBUG_FLAGS := -O0 -DDEBUG -fno-inline-functions -fno-omit-frame-pointer
DEMO_DIR:=./demo

export BUILDTYPE ?= Release

export GEOMETRY_RELEASE ?= 1.0.0
export PROTOZERO_RELEASE ?= 1.5.1
export VARIANT_RELEASE ?= 1.1.4

ifeq ($(BUILDTYPE),Release)
	FINAL_FLAGS := $(WARNING_FLAGS) $(RELEASE_FLAGS)
else
	FINAL_FLAGS := -g $(WARNING_FLAGS) $(DEBUG_FLAGS)
endif

default: test

HEADERS = $(wildcard include/mapbox/vector_tile/*.hpp) include/mapbox/vector_tile.hpp

./.mason/mason:
	mkdir ./.mason
	curl -sSfL https://github.com/mapbox/mason/archive/v0.19.0.tar.gz | tar --gunzip --extract --strip-components=1 --exclude="*md" --exclude="test*" --directory=./.mason

mason_packages/headers/protozero: .mason/mason
	.mason/mason install protozero $(PROTOZERO_RELEASE) && .mason/mason link protozero $(PROTOZERO_RELEASE)

mason_packages/headers/geometry: .mason/mason
	.mason/mason install geometry $(GEOMETRY_RELEASE) && .mason/mason link geometry $(GEOMETRY_RELEASE)

mason_packages/headers/variant: .mason/mason
	.mason/mason install variant $(VARIANT_RELEASE) && .mason/mason link variant $(VARIANT_RELEASE)

deps: mason_packages/headers/geometry mason_packages/headers/variant mason_packages/headers/protozero

build/$(BUILDTYPE)/test: test/unit/* $(HEADERS) Makefile
	mkdir -p build/$(BUILDTYPE)/
	$(CXX) $(FINAL_FLAGS) test/unit/*.cpp -isystem test/include $(CXXFLAGS) -o build/$(BUILDTYPE)/test

test/mvt-fixtures:
	git submodule update --init

test: deps build/$(BUILDTYPE)/test test/mvt-fixtures
	./build/$(BUILDTYPE)/test

# added with: git submodule add https://github.com/mapbox/mvt-bench-fixtures.git bench/mvt-bench-fixtures
bench/mvt-bench-fixtures:
	git submodule update --init

build/$(BUILDTYPE)/bench: bench/* $(HEADERS) Makefile bench/mvt-bench-fixtures
	mkdir -p build/$(BUILDTYPE)/
	$(CXX) $(FINAL_FLAGS) bench/*.cpp $(CXXFLAGS) -o build/$(BUILDTYPE)/bench

bench: deps build/$(BUILDTYPE)/bench
	./build/$(BUILDTYPE)/bench

debug:
	BUILDTYPE=Debug make test

COMMON_DOC_FLAGS = --report --output docs $(HEADERS)

clean:
	rm -rf build/
	rm -rf demo/data/
	rm -rf demo/include/

distclean: clean
	rm -rf .mason
	rm -rf mason_packages

cldoc:
	pip install cldoc --user

testpack:
	rm -f ./*tgz
	npm pack
	tar -ztvf *tgz
	rm -f ./*tgz

demo:
	rm -rf $(DEMO_DIR)/include
	rm -rf $(DEMO_DIR)/data
	rm -rf $(DEMO_DIR)/decode
	mkdir -p $(DEMO_DIR)/include/
	mkdir -p $(DEMO_DIR)/data/
	cp -r include/* $(DEMO_DIR)/include/
	cp -r $(shell .mason/mason prefix geometry $(GEOMETRY_RELEASE))/include/* $(DEMO_DIR)/include/
	cp -r $(shell .mason/mason prefix variant $(VARIANT_RELEASE))/include/* $(DEMO_DIR)/include/
	cp -r $(shell .mason/mason prefix protozero $(PROTOZERO_RELEASE))/include/* $(DEMO_DIR)/include/
	cp test/mvt-fixtures/fixtures/valid/* $(DEMO_DIR)/data/

run-demo: demo
	make -C $(DEMO_DIR)
	./demo/decode ./demo/data/Feature-single-point.mvt

docs: cldoc
	cldoc generate $(CXXFLAGS) -- $(COMMON_DOC_FLAGS)

.PHONY: demo run-demo