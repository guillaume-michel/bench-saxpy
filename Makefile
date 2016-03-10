all: saxpy saxpy-scalar

saxpy: saxpy.cpp
	g++ -std=c++14 -O3 saxpy.cpp -o saxpy

saxpy-scalar: saxpy.cpp
	g++ -std=c++14 -O3 saxpy.cpp -fno-vectorize -o saxpy-scalar

asm: saxpy.cpp
	g++ -std=c++14 -O3 -S saxpy.cpp
clean:
	rm -rf *.o *.s saxpy saxpy-scalar
