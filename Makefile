CXXFLAGS += -I include/mapbox/vector_tile --std=c++14 -Wall -Wextra -Werror 

default: test

test: tests/unit/* include/mapbox/vector_tile/* Makefile
	$(CXX) tests/unit/vector_tile*.cpp $(CXXFLAGS) -o test
	./test

SOURCES = $(include/mapbox/vector_tile/vector_tile.hpp)
HEADERS = $(wildcard include/mapbox/vector_tile/*.hpp)
COMMON_DOC_FLAGS = --report --output docs $(HEADERS)

cldoc:
	pip install cldoc --user

docs: cldoc
	cldoc generate $(CXXFLAGS) -- $(COMMON_DOC_FLAGS)