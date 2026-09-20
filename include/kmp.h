#ifndef KMP_H
#define KMP_H

#include <string>
#include <vector>

#include "pattern_matching.h"

std::vector<int> buildFailureFunction(
    const std::string& pattern
);

SearchResult kmpSearch(
    const std::string& text,
    const std::string& pattern,
    bool recordSteps = false
);

#endif