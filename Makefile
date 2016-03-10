CXX=clang++
FLAGS=-std=c++14 -Ofast
SFLAGS=$(FLAGS) -fno-tree-vectorize

all: saxpy saxpy-scalar

saxpy: saxpy.cpp
	$(CXX) $(FLAGS) saxpy.cpp -o saxpy

saxpy-scalar: saxpy.cpp
	$(CXX) $(SFLAGS) saxpy.cpp -o saxpy-scalar

asm: saxpy.cpp
	$(CXX) $(FLAGS) -S saxpy.cpp

asm-scalar: saxpy.cpp
	$(CXX) $(SFLAGS) -S saxpy.cpp

clean:
	rm -rf *.o *.s saxpy saxpy-scalar
