CXX=clang++
FLAGS=-std=c++14 -Ofast
SFLAGS=$(FLAGS) -fno-tree-vectorize

all: saxpy saxpy-scalar saxpy-nopt

saxpy: saxpy.cpp
	$(CXX) $(FLAGS) saxpy.cpp -o saxpy

saxpy-scalar: saxpy.cpp
	$(CXX) $(SFLAGS) saxpy.cpp -o saxpy-scalar

saxpy-nopt: saxpy.cpp
	$(CXX) -std=c++14 -O0 saxpy.cpp -o saxpy-nopt

asm: saxpy.cpp
	$(CXX) $(FLAGS) -S saxpy.cpp

asm-scalar: saxpy.cpp
	$(CXX) $(SFLAGS) -S saxpy.cpp

clean:
	rm -rf *.o *.s saxpy saxpy-scalar saxpy-nopt
