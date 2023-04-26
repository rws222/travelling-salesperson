CXX		 = g++
CXXFLAGS = -Wall -g -O3 -std=c++17 -pthread
TARGET	 = bruteForce complete parallel_complete parallel_bruteForce
SRC	 = bruteForce.cc complete.cc parallel_complete.cc parallel_bruteForce.cc
TBB  = -ltbb

all: bruteForce complete parallel_complete parallel_bruteForce

bruteForce: bruteForce.cc
	$(CXX) $(CXXFLAGS) $< -o $@

complete: complete.cc
	$(CXX) $(CXXFLAGS)  $< -o $@

parallel_complete: parallel_complete.cc
	$(CXX) $(CXXFLAGS)  $< -o $@ -Loneapi-tbb-2021.7.0/lib/intel64/gcc4.8 -ltbb

parallel_bruteForce: parallel_bruteForce.cc
	$(CXX) $(CXXFLAGS)  $< -o $@ -Loneapi-tbb-2021.7.0/lib/intel64/gcc4.8 -ltbb

clean:
	rm -f $(TARGET)
