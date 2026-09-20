#ifndef PATTERN_MATCHING_H
#define PATTERN_MATCHING_H

#include <string>
#include <vector>

enum class SearchEventType {
    Comparison,
    Shift,
    Fallback,
    HashCheck,
    Candidate,
    Collision,
    Found
};

struct SearchStep {
    SearchEventType type;

    int textIndex = -1;
    int patternIndex = -1;

    int alignment = 0;
    int shift = 0;

    int nextPatternIndex = -1;

    bool matches = false;

    long long textHash = 0;
    long long patternHash = 0;

    std::string message;
};

struct SearchResult {
    int position;
    long long comparisons;
    std::vector<SearchStep> steps;

    SearchResult()
        : position(-1), comparisons(0) {}
};

#endif