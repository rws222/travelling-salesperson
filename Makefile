CXX		 = g++
CXXFLAGS = -Wall -g -O3 -std=c++17 -pthread
TARGET	 = bruteForce complete complete_parallel_distance_comp
SRC	 = bruteForce.cc complete.cc complete_parallel_distance_comp.cc
TBB  = -ltbb

all: bruteForce complete complete_parallel_distance_comp

bruteForce: bruteForce.cc
	$(CXX) $(CXXFLAGS) $< -o $@

complete: complete.cc
	$(CXX) $(CXXFLAGS)  $< -o $@

complete_parallel_distance_comp: complete_parallel_distance_comp.cc
	$(CXX) $(CXXFLAGS)  $< -o $@ -Loneapi-tbb-2021.7.0/lib/intel64/gcc4.8 -ltbb

clean:
	rm -f $(TARGET)
