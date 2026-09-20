#include "boyer_moore.h"

#include <algorithm>

std::vector<int> buildLastOccurrenceTable(
    const std::string& pattern
) {
    constexpr int alphabetSize = 256;

    std::vector<int> lastOccurrence(
        alphabetSize,
        -1
    );

    for (
        int i = 0;
        i < static_cast<int>(pattern.size());
        ++i
    ) {
        const unsigned char character =
            static_cast<unsigned char>(pattern[i]);

        lastOccurrence[character] = i;
    }

    return lastOccurrence;
}

SearchResult boyerMooreSearch(
    const std::string& text,
    const std::string& pattern,
    bool recordSteps
) {
    SearchResult result;

    const int n = static_cast<int>(text.size());
    const int m = static_cast<int>(pattern.size());

    if (m == 0) {
        result.position = 0;
        return result;
    }

    if (m > n) {
        return result;
    }

    const std::vector<int> lastOccurrence =
        buildLastOccurrenceTable(pattern);

    int alignment = 0;

    while (alignment <= n - m) {
        int j = m - 1;

        while (j >= 0) {
            ++result.comparisons;

            const bool matches =
                pattern[j] == text[alignment + j];

            if (recordSteps) {
                SearchStep step;

                step.type = SearchEventType::Comparison;
                step.textIndex = alignment + j;
                step.patternIndex = j;
                step.alignment = alignment;
                step.matches = matches;

                step.message = matches
                    ? "Characters match"
                    : "Characters do not match";

                result.steps.push_back(step);
            }

            if (!matches) {
                break;
            }

            --j;
        }

        if (j < 0) {
            result.position = alignment;

            if (recordSteps) {
                SearchStep step;

                step.type = SearchEventType::Found;
                step.alignment = alignment;
                step.matches = true;
                step.message = "Pattern found";

                result.steps.push_back(step);
            }

            return result;
        }

        const unsigned char badCharacter =
            static_cast<unsigned char>(
                text[alignment + j]
            );

        const int lastIndex =
            lastOccurrence[badCharacter];

        const int patternShift =
            std::max(
                1,
                j - lastIndex
            );

        if (recordSteps) {
            SearchStep step;

            step.type = SearchEventType::Shift;
            step.textIndex = alignment + j;
            step.patternIndex = j;
            step.alignment = alignment;
            step.shift = patternShift;

            step.message =
                "Apply bad-character shift";

            result.steps.push_back(step);
        }

        alignment += patternShift;
    }

    return result;
}