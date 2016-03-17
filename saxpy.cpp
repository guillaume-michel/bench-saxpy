#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>

inline constexpr size_t minimum_common_size() {
    return 0u;
}

template<class SizedSequence>
constexpr size_t minimum_common_size(const SizedSequence &first) {
    return first.size();
}

template<class Seq, class ...MoreSeqs>
constexpr std::size_t minimum_common_size(const Seq &first, const MoreSeqs &...rest) {
    return std::min<std::size_t>(first.size(),
                                 minimum_common_size(rest...));
}

template<typename OutIter, typename Func, class ...SizedSequences>
OutIter zip_with(OutIter o, Func &&func, SizedSequences &&...containers) {
    auto const s = minimum_common_size(containers...);

    for (std::size_t i = 0 ; i < s ; ++i) {
        *o++ = func(containers[i]...);
    }

    return o;
}

template<typename F, typename T, typename Arg>
auto fold(F f, T&& t, Arg&& a) {//-> decltype(f(std::forward<T>(t), std::forward<Arg>(a))) {
    return f(std::forward<T>(t), std::forward<Arg>(a));
}

template<typename F, typename T, typename Head, typename... Args>
auto fold(F f, T&& init, Head&& h, Args&&... args) {// -> decltype(f(std::forward<T>(init), std::forward<Head>(h))) {
    return fold(f,
                f(std::forward<T>(init), std::forward<Head>(h)),
                std::forward<Args>(args)...);
}

// hack in a fold for void functions
struct ignore {};

// cannot be a lambda, needs to be polymorphic on the iterator type
struct end_or {
    template<typename InputIterator>
    bool operator()(bool in, const std::pair<InputIterator, InputIterator>& p){
        return in || p.first == p.second;
    }
};

// same same but different
struct inc {
    template<typename InputIterator>
    ignore operator()(ignore, std::pair<InputIterator, InputIterator>& p) {
        p.first++; return ignore();
    }
};

template<typename Fun, typename OutputIterator, typename... InputIterators>
void ZipWith(Fun f, OutputIterator out, std::pair<InputIterators, InputIterators>... inputs) {

    while(!fold(end_or(), false, inputs...)) {
        *out++ = f( *(inputs.first)... );
        fold(inc(), ignore(), inputs...);
    }
}

template<typename T>
void saxpy(std::vector<T> &z, const T a, const std::vector<T> &x, const std::vector<T> &y) {

    for (size_t i=0; i<x.size(); ++i) {
        z[i] = a*x[i] + y[i];
    }
}

int main(int argc, char *argv[]) {

    using Type = float;

    //const size_t N = 1000000000;
    const size_t N = 100000000;
    std::vector<Type> z(N, 0);
    std::vector<Type> x(N, 1);
    std::vector<Type> y(N, 2);
    const Type a = 2.5;

    {
        auto start = std::chrono::high_resolution_clock::now();
        saxpy(z, a, x, y);
        auto end = std::chrono::high_resolution_clock::now();

        float duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()/1e6;
        std::cout << std::fixed << std::setprecision(6) << duration << " ms" << std::endl;
        std::cout << "MFLOPS: " << (float)(N*2)/(float)(1000000*duration/1000) << std::endl;
    }

    {
        std::cout << "zip_with implementation:" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        zip_with(std::begin(z),
                 [a](auto xi, auto yi) {
                     return a*xi + yi;
                 },
                 x,
                 y);
        auto end = std::chrono::high_resolution_clock::now();

        float duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()/1e6;
        std::cout << std::fixed << std::setprecision(6) << duration << " ms" << std::endl;
        std::cout << "MFLOPS: " << (float)(N*2)/(float)(1000000*duration/1000) << std::endl;
    }

    {
        std::cout << "ZipWith implementation:" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        ZipWith([a](auto xi, auto yi) {
                     return a*xi + yi;
            },
            std::begin(z),
            std::make_pair(std::begin(x), std::end(x)),
            std::make_pair(std::begin(y), std::end(y)));

        auto end = std::chrono::high_resolution_clock::now();

        float duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()/1e6;
        std::cout << std::fixed << std::setprecision(6) << duration << " ms" << std::endl;
        std::cout << "MFLOPS: " << (float)(N*2)/(float)(1000000*duration/1000) << std::endl;
    }

    return (int)z[0]; // this to force the compiler to really compute z
}
