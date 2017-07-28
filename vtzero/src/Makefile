
PROTOZERO_INCLUDE := ../../../protozero/include

CXXFLAGS := -I../include -I${PROTOZERO_INCLUDE} -g -Wall -Wextra -Werror

PRGS := vtzero-create vtzero-show vtzero-filter

all: $(PRGS)

utils.o: utils.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -c $^ -o $@

vtzero-create.o: vtzero-create.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -c $^ -o $@

vtzero-create: vtzero-create.o
	$(CXX) $^ -o $@

vtzero-filter.o: vtzero-filter.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -c $^ -o $@

vtzero-filter: vtzero-filter.o utils.o
	$(CXX) $^ -o $@

vtzero-show.o: vtzero-show.cpp
	$(CXX) $(CXXFLAGS) -std=c++11 -c $^ -o $@

vtzero-show: vtzero-show.o utils.o
	$(CXX) $^ -o $@

clean:
	rm -f $(PRGS) *.o

