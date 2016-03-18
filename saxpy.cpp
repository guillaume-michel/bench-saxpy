#include <vector>
#include <string>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <functional>

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

template<typename T, typename Lambda>
std::vector<std::pair<size_t, float>> bench(size_t M, std::vector<size_t> &Ns, Lambda &&lambda) {

    T value = 0; // to avoid optimization of the loop by the compiler

    std::vector<std::pair<size_t, float>> results;

    for (auto N: Ns) {
        std::vector<T> z(N, 0);
        std::vector<T> x(N, 1);
        std::vector<T> y(N, 2);
        const T a = 2.5;

        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i=0; i<M; ++i) {
            lambda(z, a, x, y);
            value += z[0];
        }
        auto end = std::chrono::high_resolution_clock::now();

        float duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()/1e6;
        //std::cout << N << ", " << std::fixed << std::setprecision(6) << duration/M << " ms" << std::endl;

        results.push_back({N, duration/M});
    }

    return results;
}

template<typename T>
using BenchItemList = std::vector<std::pair<std::string,
                                            std::function<void (std::vector<T> &z,
                                                                const T,
                                                                const std::vector<T> &,
                                                                const std::vector<T> &)>>>;

template<typename T>
BenchItemList<T> getBenchItems() {
    BenchItemList<T> benchItems = {
        {"C++ standard for loop:",
         [](std::vector<T> &z, const T a, const std::vector<T> &x, const std::vector<T> &y) {
                saxpy(z, a, x, y);
            }
        },
        {"C++ zip_with implementation:",
         [](std::vector<T> &z, const T a, const std::vector<T> &x, const std::vector<T> &y) {
                zip_with(std::begin(z),
                         [a](auto xi, auto yi) {
                             return a*xi + yi;
                         },
                         x,
                         y);
            }
        },
        {"C++ ZipWith implementation:",
         [](std::vector<T> &z, const T a, const std::vector<T> &x, const std::vector<T> &y) {
                ZipWith([a](auto xi, auto yi) {
                        return a*xi + yi;
                    },
                    std::begin(z),
                    std::make_pair(std::begin(x), std::end(x)),
                    std::make_pair(std::begin(y), std::end(y)));
            }
        }

    };

    return benchItems;
}

int main(int argc, char *argv[]) {

    using Type = double;

    std::string csvSeparator = ", ";

    size_t M = 10;
    std::vector<size_t> Ns = {10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    auto benchItems = getBenchItems<Type>();

    std::vector<std::string> headers = {"\"Size\""};
    std::vector<std::pair<size_t, std::vector<float>>> results;

    // init results
    for (auto N: Ns) {
        results.push_back({N, std::vector<float>()});
    }

    for (auto benchItem: benchItems) {
        std::cout << "Running Benchmark for " << benchItem.first;
        auto itemResults = bench<Type>(M, Ns, benchItem.second);

        headers.push_back(std::string("\"") + benchItem.first + " (ms)\"");

        for (size_t i=0; i<itemResults.size(); ++i) {
            auto N = itemResults[i].first;
            auto duration = itemResults[i].second;
            results[i].second.push_back(duration);
        }

        std::cout << " DONE" << std::endl;
    }

    std::cout << "Results: " << std::endl;
    // print CSV headers
    for (size_t i=0; i<headers.size(); ++i) {
        std::cout << headers[i];

        if (i != headers.size() - 1) {
            std::cout << csvSeparator;
        } else {
            std::cout << std::endl;
        }
    }

    // print CSV data
    for (auto result: results) {
        auto N = result.first;
        auto durations = result.second;
        std::cout << N << csvSeparator;

        for (size_t i=0; i<durations.size(); ++i) {
            std::cout << std::fixed << std::setprecision(6) << durations[i];

            if (i != durations.size() - 1) {
                std::cout << csvSeparator;
            } else {
                std::cout << std::endl;
            }
        }
    }

    return 0;
}
