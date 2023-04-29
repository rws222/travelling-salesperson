### before compiling, run the following ###
# module load gcc-11.2.0
# cd oneapi-tbb-2021.8.0/env
# source vars.sh
# cd ../..

CXX		 = g++
CXXFLAGS = -Wall -g -O3 -std=c++17 -pthread -Iinclude -Loneapi-tbb-2021.8.0/lib/intel64/gcc4.8 -ltbb -mavx2 
TARGET	 = bruteForce bruteForce_vector_ins complete parallel_complete parallel_bruteForce 3opt 3opt_vector_ins
SRC	 = bruteForce.cc bruteForce_vector_ins.cc complete.cc parallel_complete.cc parallel_bruteForce.cc 3opt.cc 3opt_vector_ins.cc
TBB  = -ltbb

all: bruteForce bruteForce_vector_ins complete parallel_complete parallel_bruteForce 3opt 3opt_vector_ins

build:
	mkdir -p build

bruteForce: bruteForce.cc build
	$(CXX) $(CXXFLAGS) $< -o build/$@

bruteForce_vector_ins: bruteForce_vector_ins.cc build
	$(CXX) $(CXXFLAGS) $< -o build/$@

complete: complete.cc build
	$(CXX) $(CXXFLAGS)  $< -o build/$@

parallel_complete: parallel_complete.cc build
	$(CXX) $(CXXFLAGS)  $< -o build/$@ 

parallel_bruteForce: parallel_bruteForce.cc build
	$(CXX) $(CXXFLAGS)  $< -o build/$@ 

3opt: 3opt.cc build
	$(CXX) $(CXXFLAGS)  $< -o build/$@ 

3opt_vector_ins: 3opt_vector_ins.cc build
	$(CXX) $(CXXFLAGS)  $< -o build/$@ 

parallel_3opt: parallel_3opt.cc build 
	$(CXX) $(CXXFLAGS)  $< -o build/$@ 

clean:
	rm -rf build



# CXX		 = g++
# CXXFLAGS = -Wall -g -O3 -std=c++17 -pthread -Iinclude -Loneapi-tbb-2021.8.0/lib/intel64/gcc4.8 -ltbb -mavx2 
# TARGET	 = bruteForce bruteForce_vector_ins complete parallel_complete parallel_bruteForce 3opt 3opt_vector_ins
# SRC	 = bruteForce.cc bruteForce_vector_ins.cc complete.cc parallel_complete.cc parallel_bruteForce.cc 3opt.cc 3opt_vector_ins.cc
# TBB  = -ltbb

# all: bruteForce bruteForce_vector_ins complete parallel_complete parallel_bruteForce 3opt 3opt_vector_ins

# bruteForce: bruteForce.cc
# 	$(CXX) $(CXXFLAGS) $< -o $@

# bruteForce_vector_ins: bruteForce_vector_ins.cc
# 	$(CXX) $(CXXFLAGS) $< -o $@

# complete: complete.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@

# parallel_complete: parallel_complete.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@ 

# parallel_bruteForce: parallel_bruteForce.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@ 

# 3opt: 3opt.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@ 

# 3opt_vector_ins: 3opt_vector_ins.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@ 

# clean:
# 	rm -f $(TARGET)



# CXX		 = g++
# CXXFLAGS = -Wall -g -O3 -std=c++17 -pthread
# TARGET	 = bruteForce complete parallel_complete parallel_bruteForce
# SRC	 = bruteForce.cc complete.cc parallel_complete.cc parallel_bruteForce.cc
# TBB  = -ltbb

# all: bruteForce bruteForce_vector_ins complete parallel_complete parallel_bruteForce 3opt 3opt_vector_ins vector_complete

# bruteForce: bruteForce.cc
# 	$(CXX) $(CXXFLAGS) $< -o $@

# bruteForce_vector_ins: bruteForce_vector_ins.cc
# 	$(CXX) $(CXXFLAGS) $< -o $@ -mavx2 

# complete: complete.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@

# parallel_complete: parallel_complete.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@ -Loneapi-tbb-2021.7.0/lib/intel64/gcc4.8 -ltbb

# parallel_bruteForce: parallel_bruteForce.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@ -Loneapi-tbb-2021.7.0/lib/intel64/gcc4.8 -ltbb

# 3opt: 3opt.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@ 

# 3opt_vector_ins: 3opt_vector_ins.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@ -mavx2 

# vector_complete: vector_complete.cc
# 	$(CXX) $(CXXFLAGS)  $< -o $@  -mavx2 

# clean:
# 	rm -f $(TARGET)
