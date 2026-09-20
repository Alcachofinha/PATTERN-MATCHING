#include "rabin_karp.h"

namespace {

constexpr long long HASH_BASE = 256;
constexpr long long HASH_MOD = 1000000007;

}

long long computeHash(
    const std::string& value
) {
    long long hash = 0;

    for (unsigned char character : value) {
        hash =
            (
                hash * HASH_BASE
                + character
            )
            % HASH_MOD;
    }

    return hash;
}

SearchResult rabinKarpSearch(
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

    long long highestPower = 1;

    for (int i = 0; i < m - 1; ++i) {
        highestPower =
            (
                highestPower * HASH_BASE
            )
            % HASH_MOD;
    }

    const long long patternHash =
        computeHash(pattern);

    long long windowHash =
        computeHash(text.substr(0, m));

    for (int i = 0; i <= n - m; ++i) {
        if (recordSteps) {
            SearchStep step;

            step.type = SearchEventType::HashCheck;
            step.textIndex = i;
            step.alignment = i;

            step.textHash = windowHash;
            step.patternHash = patternHash;

            step.matches =
                windowHash == patternHash;

            step.message =
                "Compare window hash with pattern hash";

            result.steps.push_back(step);
        }

        if (windowHash == patternHash) {
            if (recordSteps) {
                SearchStep step;

                step.type = SearchEventType::Candidate;
                step.alignment = i;
                step.matches = true;

                step.textHash = windowHash;
                step.patternHash = patternHash;

                step.message =
                    "Hash match: verify candidate";

                result.steps.push_back(step);
            }

            bool fullMatch = true;

            for (int j = 0; j < m; ++j) {
                ++result.comparisons;

                const bool matches =
                    text[i + j] == pattern[j];

                if (recordSteps) {
                    SearchStep step;

                    step.type =
                        SearchEventType::Comparison;

                    step.textIndex = i + j;
                    step.patternIndex = j;
                    step.alignment = i;
                    step.matches = matches;

                    step.message = matches
                        ? "Candidate character matches"
                        : "Candidate character does not match";

                    result.steps.push_back(step);
                }

                if (!matches) {
                    fullMatch = false;

                    if (recordSteps) {
                        SearchStep step;

                        step.type =
                            SearchEventType::Collision;

                        step.alignment = i;

                        step.textHash = windowHash;
                        step.patternHash = patternHash;

                        step.message =
                            "Hash collision detected";

                        result.steps.push_back(step);
                    }

                    break;
                }
            }

            if (fullMatch) {
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
        }

        if (i < n - m) {
            if (recordSteps) {
                SearchStep step;

                step.type = SearchEventType::Shift;
                step.alignment = i;
                step.shift = 1;
                step.message =
                    "Move rolling hash window";

                result.steps.push_back(step);
            }

            const unsigned char outgoingCharacter =
                static_cast<unsigned char>(
                    text[i]
                );

            const unsigned char incomingCharacter =
                static_cast<unsigned char>(
                    text[i + m]
                );

            windowHash =
                (
                    windowHash
                    - outgoingCharacter * highestPower
                )
                % HASH_MOD;

            if (windowHash < 0) {
                windowHash += HASH_MOD;
            }

            windowHash =
                (
                    windowHash * HASH_BASE
                    + incomingCharacter
                )
                % HASH_MOD;
        }
    }

    return result;
}