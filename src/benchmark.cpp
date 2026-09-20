#include "benchmark.h"

#include <chrono>

#include "brute_force.h"
#include "boyer_moore.h"
#include "kmp.h"
#include "rabin_karp.h"

namespace {

using SearchFunction = SearchResult (*)(
    const std::string&,
    const std::string&,
    bool
);

BenchmarkResult measureAlgorithm(
    const std::string& algorithmName,
    SearchFunction searchFunction,
    const std::string& text,
    const std::vector<std::string>& patterns
) {
    int matchesFound = 0;

    const auto start =
        std::chrono::high_resolution_clock::now();

    for (const std::string& pattern : patterns) {
        const SearchResult result =
            searchFunction(text, pattern, false);

        if (result.position != -1) {
            ++matchesFound;
        }
    }

    const auto end =
        std::chrono::high_resolution_clock::now();

    const std::chrono::duration<double, std::milli> elapsed =
        end - start;

    return {
        algorithmName,
        elapsed.count(),
        matchesFound
    };
}

} 

std::vector<BenchmarkResult> runBenchmark(
    const std::string& text,
    const std::vector<std::string>& patterns
) {
    std::vector<BenchmarkResult> results;

    results.push_back(
        measureAlgorithm(
            "Brute Force",
            bruteForceSearch,
            text,
            patterns
        )
    );

    results.push_back(
        measureAlgorithm(
            "Boyer-Moore",
            boyerMooreSearch,
            text,
            patterns
        )
    );

    results.push_back(
        measureAlgorithm(
            "KMP",
            kmpSearch,
            text,
            patterns
        )
    );

    results.push_back(
        measureAlgorithm(
            "Rabin-Karp",
            rabinKarpSearch,
            text,
            patterns
        )
    );

    return results;
}