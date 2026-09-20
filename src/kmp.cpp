#include "kmp.h"

std::vector<int> buildFailureFunction(
    const std::string& pattern
) {
    const int m =
        static_cast<int>(pattern.size());

    if (m == 0) {
        return {};
    }

    std::vector<int> failure(m, 0);

    int prefixLength = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[prefixLength]) {
            ++prefixLength;

            failure[i] = prefixLength;

            ++i;
        } else if (prefixLength > 0) {
            prefixLength =
                failure[prefixLength - 1];
        } else {
            failure[i] = 0;

            ++i;
        }
    }

    return failure;
}

SearchResult kmpSearch(
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

    const std::vector<int> failure =
        buildFailureFunction(pattern);

    int textIndex = 0;
    int patternIndex = 0;

    while (textIndex < n) {
        ++result.comparisons;

        const bool matches =
            text[textIndex] == pattern[patternIndex];

        if (recordSteps) {
            SearchStep step;

            step.type = SearchEventType::Comparison;
            step.textIndex = textIndex;
            step.patternIndex = patternIndex;
            step.alignment =
                textIndex - patternIndex;

            step.matches = matches;

            step.message = matches
                ? "Characters match"
                : "Characters do not match";

            result.steps.push_back(step);
        }

        if (matches) {
            ++textIndex;
            ++patternIndex;

            if (patternIndex == m) {
                result.position =
                    textIndex - m;

                if (recordSteps) {
                    SearchStep step;

                    step.type = SearchEventType::Found;
                    step.alignment = result.position;
                    step.matches = true;
                    step.message = "Pattern found";

                    result.steps.push_back(step);
                }

                return result;
            }
        } else if (patternIndex > 0) {
            const int newPatternIndex =
                failure[patternIndex - 1];

            if (recordSteps) {
                SearchStep step;

                step.type = SearchEventType::Fallback;

                step.textIndex = textIndex;
                step.patternIndex = patternIndex;

                step.alignment =
                    textIndex - patternIndex;

                step.shift =
                    patternIndex - newPatternIndex;

                step.nextPatternIndex =
                    newPatternIndex;

                step.message =
                    "Use failure function";

                result.steps.push_back(step);
            }

            patternIndex =
                newPatternIndex;
        } else {
            if (recordSteps) {
                SearchStep step;

                step.type = SearchEventType::Shift;

                step.textIndex = textIndex;
                step.patternIndex = patternIndex;
                step.alignment = textIndex;
                step.shift = 1;

                step.message =
                    "Advance text by one position";

                result.steps.push_back(step);
            }

            ++textIndex;
        }
    }

    return result;
}