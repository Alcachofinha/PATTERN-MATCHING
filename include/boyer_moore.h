#ifndef BOYER_MOORE_H
#define BOYER_MOORE_H

#include <string>
#include <vector>

#include "pattern_matching.h"

std::vector<int> buildLastOccurrenceTable(
    const std::string& pattern
);

SearchResult boyerMooreSearch(
    const std::string& text,
    const std::string& pattern,
    bool recordSteps = false
);

#endif