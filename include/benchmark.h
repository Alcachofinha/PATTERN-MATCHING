#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <vector>

struct BenchmarkResult {
    std::string algorithm;
    double timeMilliseconds;
    int matchesFound;
};

std::vector<BenchmarkResult> runBenchmark(
    const std::string& text,
    const std::vector<std::string>& patterns
);

#endif