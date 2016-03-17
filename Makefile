CXX=clang++
FLAGS=-std=c++14 -Ofast
SFLAGS=$(FLAGS) -fno-tree-vectorize

all: saxpy saxpy-scalar saxpy-nopt ocaml lisp

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

ocaml: saxpy.ml
	corebuild saxpy.native

lisp: saxpy.lisp
	buildapp --output saxpy_lisp --entry main --load saxpy.lisp --dynamic-space-size 14000

test: all
	-./saxpy
	-./saxpy.native
	-./saxpy_lisp

clean:
	rm -rf *.o *.s saxpy saxpy-scalar saxpy-nopt saxpy.native _build saxpy_lisp
