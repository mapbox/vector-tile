
PROTOZERO_INCLUDE := ../../../../mapbox/protozero/include

CXXFLAGS := -Iinclude -I../include -I${PROTOZERO_INCLUDE} -g -Wall -Wextra -Werror

PRGS := unit_tests

all: $(PRGS)

unit_tests: unit/test_main.o unit/tests.o
	$(CXX) $^ -o $@

unit/test_main.o: unit/test_main.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -c $^ -o $@

unit/tests.o: unit/tests.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -c $^ -o $@

clean:
	rm -f $(PRGS) *.o

