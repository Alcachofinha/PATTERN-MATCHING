#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <string>

#include "pattern_matching.h"

SearchResult bruteForceSearch(
    const std::string& text,
    const std::string& pattern,
    bool recordSteps = false
);

#endif