#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>

template<typename T>
void saxpy(std::vector<T> &z, const T a, const std::vector<T> &x, const std::vector<T> &y) {

    for (size_t i=0; i<x.size(); ++i) {
        z[i] = a*x[i] + y[i];
    }
}

int main(int argc, char *argv[]) {

    using Type = float;

    //const size_t N = 1000000000;
    const size_t N = 10000000;
    std::vector<Type> z(N, 0);
    std::vector<Type> x(N, 1);
    std::vector<Type> y(N, 2);
    const Type a = 2.5;

    auto start = std::chrono::high_resolution_clock::now();
    saxpy(z, a, x, y);
    auto end = std::chrono::high_resolution_clock::now();

    float duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()/1e6;
    std::cout << std::fixed << std::setprecision(6) << duration << " ms" << std::endl;
    std::cout << "MFLOPS: " << (float)(N*2)/(float)(1000000*duration/1000) << std::endl;
    return (int)z[0]; // this to force the compiler to really compute z
}
