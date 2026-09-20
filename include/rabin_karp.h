#ifndef RABIN_KARP_H
#define RABIN_KARP_H

#include <string>

#include "pattern_matching.h"

long long computeHash(
    const std::string& value
);

SearchResult rabinKarpSearch(
    const std::string& text,
    const std::string& pattern,
    bool recordSteps = false
);

#endif