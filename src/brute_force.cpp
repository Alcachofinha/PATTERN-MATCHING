#include "brute_force.h"

SearchResult bruteForceSearch(
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

    for (int i = 0; i <= n - m; ++i) {
        int j = 0;

        while (j < m) {
            ++result.comparisons;

            const bool matches =
                text[i + j] == pattern[j];

            if (recordSteps) {
                SearchStep step;

                step.type = SearchEventType::Comparison;
                step.textIndex = i + j;
                step.patternIndex = j;
                step.alignment = i;
                step.matches = matches;
                step.message = matches
                    ? "Characters match"
                    : "Characters do not match";

                result.steps.push_back(step);
            }

            if (!matches) {
                break;
            }

            ++j;
        }

        if (j == m) {
            result.position = i;

            if (recordSteps) {
                SearchStep step;

                step.type = SearchEventType::Found;
                step.alignment = i;
                step.matches = true;
                step.message = "Pattern found";

                result.steps.push_back(step);
            }

            return result;
        }

        if (recordSteps && i < n - m) {
            SearchStep step;

            step.type = SearchEventType::Shift;
            step.alignment = i;
            step.shift = 1;
            step.message = "Shift pattern by one position";

            result.steps.push_back(step);
        }
    }

    return result;
}