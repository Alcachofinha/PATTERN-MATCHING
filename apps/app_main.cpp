#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cctype>

#include "benchmark.h"
#include "boyer_moore.h"
#include "brute_force.h"
#include "kmp.h"
#include "rabin_karp.h"


// TYPES


struct TestCase {
    std::string text;
    std::string pattern;
    int expectedPosition;
};


using SearchAlgorithm = SearchResult (*)(
    const std::string&,
    const std::string&,
    bool
);



// GENERAL OUTPUT


std::string toUpperCase(const std::string& text) {
    std::string result = text;

    for (char& character : result) {
        character = static_cast<char>(
            std::toupper(
                static_cast<unsigned char>(character)
            )
        );
    }

    return result;
}


void printTitle(const std::string& title) {
    std::cout
        << "\n- "
        << toUpperCase(title)
        << " -\n\n";
}


std::string eventTypeToString(
    SearchEventType type
) {
    switch (type) {

        case SearchEventType::Comparison:
            return "COMPARE";

        case SearchEventType::Shift:
            return "SHIFT";

        case SearchEventType::Fallback:
            return "FALLBACK";

        case SearchEventType::HashCheck:
            return "HASH_CHECK";

        case SearchEventType::Candidate:
            return "CANDIDATE";

        case SearchEventType::Collision:
            return "COLLISION";

        case SearchEventType::Found:
            return "FOUND";
    }

    return "UNKNOWN";
}



// SEARCH TRACE


void printSearchTrace(
    const SearchResult& result
) {
    std::cout << "\nExecution trace:\n\n";

    for (
        std::size_t i = 0;
        i < result.steps.size();
        ++i
    ) {
        const SearchStep& step =
            result.steps[i];

        std::cout
            << std::setw(3)
            << i + 1
            << ". "
            << std::left
            << std::setw(12)
            << eventTypeToString(step.type);

        if (step.textIndex >= 0) {
            std::cout
                << " T["
                << step.textIndex
                << "]";
        }

        if (step.patternIndex >= 0) {
            std::cout
                << " P["
                << step.patternIndex
                << "]";
        }

        std::cout
            << " | alignment="
            << step.alignment;

        if (step.shift > 0) {
            std::cout
                << " | shift="
                << step.shift;
        }

        if (step.nextPatternIndex >= 0) {
            std::cout
                << " | nextP="
                << step.nextPatternIndex;
        }

        std::cout
            << "\n     -> "
            << step.message
            << '\n';
    }
}

// AUXILIARY TABLES

void printLastOccurrenceTable(
    const std::string& pattern
) {
    const std::vector<int> table =
        buildLastOccurrenceTable(pattern);

    std::cout
        << "\nLast Occurrence Table L(x):\n";

    for (char character : pattern) {

        const unsigned char index =
            static_cast<unsigned char>(
                character
            );

        std::cout
            << "L("
            << character
            << ") = "
            << table[index]
            << '\n';
    }
}


void printFailureFunction(
    const std::string& pattern
) {
    const std::vector<int> failure =
        buildFailureFunction(pattern);

    std::cout
        << "\nFailure Function F(k):\n\n";

    std::cout
        << "Index:    ";

    for (
        std::size_t i = 0;
        i < pattern.size();
        ++i
    ) {
        std::cout
            << std::setw(3)
            << i;
    }

    std::cout
        << "\nPattern:  ";

    for (char character : pattern) {
        std::cout
            << std::setw(3)
            << character;
    }

    std::cout
        << "\nF(k):     ";

    for (int value : failure) {
        std::cout
            << std::setw(3)
            << value;
    }

    std::cout << '\n';
}

// ALGORITHM DEMOS

void printDemoResult(
    const std::string& algorithm,
    const std::string& text,
    const std::string& pattern,
    const SearchResult& result
) {
    printTitle(
        "DEMO: " + algorithm
    );

    std::cout
        << "Text:        "
        << text
        << '\n';

    std::cout
        << "Pattern:     "
        << pattern
        << '\n';

    std::cout
        << "Text size:   "
        << text.size()
        << '\n';

    std::cout
        << "Pattern size:"
        << ' '
        << pattern.size()
        << '\n';

    std::cout
        << "Comparisons: "
        << result.comparisons
        << '\n';

    if (result.position != -1) {
        std::cout
            << "Result:      Pattern found at position "
            << result.position
            << '\n';
    }
    else {
        std::cout
            << "Result:      Pattern not found\n";
    }
}


void runBruteForceDemo() {
    const std::string text =
        "abaca";

    const std::string pattern =
        "aca";

    const SearchResult result =
        bruteForceSearch(
            text,
            pattern,
            true
        );

    printDemoResult(
        "Brute Force",
        text,
        pattern,
        result
    );

    std::cout
        << "\nMain idea:\n"
        << "Mismatch -> shift pattern by exactly 1 position.\n";

    printSearchTrace(result);
}


void runBoyerMooreDemo() {
    const std::string text =
        "abacaba";

    const std::string pattern =
        "caba";

    const SearchResult result =
        boyerMooreSearch(
            text,
            pattern,
            true
        );

    printDemoResult(
        "Boyer-Moore",
        text,
        pattern,
        result
    );

    std::cout
        << "\nMain idea:\n"
        << "Compare from right to left and use L(x) "
        << "to calculate the shift.\n";

    printLastOccurrenceTable(
        pattern
    );

    printSearchTrace(result);
}


void runKMPDemo() {
    const std::string text =
        "ababaca";

    const std::string pattern =
        "abaca";

    const SearchResult result =
        kmpSearch(
            text,
            pattern,
            true
        );

    printDemoResult(
        "Knuth-Morris-Pratt",
        text,
        pattern,
        result
    );

    std::cout
        << "\nMain idea:\n"
        << "On mismatch, reuse a prefix that is also a suffix.\n"
        << "The text index does not move backwards.\n";

    printFailureFunction(
        pattern
    );

    printSearchTrace(result);
}


void runRabinKarpDemo() {
    const std::string text =
        "abracadabra";

    const std::string pattern =
        "cada";

    const SearchResult result =
        rabinKarpSearch(
            text,
            pattern,
            true
        );

    printDemoResult(
        "Rabin-Karp",
        text,
        pattern,
        result
    );

    std::cout
        << "\nMain idea:\n"
        << "Compare hashes first. "
        << "If hashes match, verify the characters.\n";

    std::cout
        << "\nPattern hash: "
        << computeHash(pattern)
        << '\n';

    printSearchTrace(result);
}


void runAlgorithmDemos() {
    runBruteForceDemo();
    runBoyerMooreDemo();
    runKMPDemo();
    runRabinKarpDemo();
}

// TESTS

void runTestsForAlgorithm(
    const std::string& algorithmName,
    SearchAlgorithm algorithm,
    const std::vector<TestCase>& tests
) {
    int passedTests = 0;

    std::cout
        << "\n"
        << algorithmName
        << '\n';

    std::cout
        << std::string(
            algorithmName.size(),
            '-'
        )
        << '\n';

    for (
        std::size_t i = 0;
        i < tests.size();
        ++i
    ) {
        const TestCase& test =
            tests[i];

        const SearchResult result =
            algorithm(
                test.text,
                test.pattern,
                false
            );

        const bool passed =
            result.position ==
            test.expectedPosition;

        if (passed) {
            ++passedTests;
        }

        std::cout
            << "Test "
            << i + 1
            << ": "
            << (passed ? "PASS" : "FAIL");

        if (!passed) {
            std::cout
                << " | expected="
                << test.expectedPosition
                << " result="
                << result.position;
        }

        std::cout << '\n';
    }

    std::cout
        << "Summary: "
        << passedTests
        << " / "
        << tests.size()
        << " passed\n";
}


void runAllTests() {
    printTitle(
        "CORRECTNESS TESTS"
    );

    const std::vector<TestCase> tests = {
        {"abaca", "aca", 2},
        {"abacaba", "caba", 3},
        {"ababaca", "abaca", 2},
        {"hello world", "world", 6},
        {"aaaaa", "aaa", 0},
        {"abcdef", "xyz", -1},
        {"abc", "abcdef", -1}
    };

    runTestsForAlgorithm(
        "Brute Force",
        bruteForceSearch,
        tests
    );

    runTestsForAlgorithm(
        "Boyer-Moore",
        boyerMooreSearch,
        tests
    );

    runTestsForAlgorithm(
        "KMP",
        kmpSearch,
        tests
    );

    runTestsForAlgorithm(
        "Rabin-Karp",
        rabinKarpSearch,
        tests
    );
}

// BENCHMARK

void runBenchmarkDemo() {
    printTitle(
        "PERFORMANCE BENCHMARK"
    );

    std::string largeText;

    const std::string block =
        "the_quick_brown_fox_jumps_over_the_lazy_dog_";

    for (
        int i = 0;
        i < 5000;
        ++i
    ) {
        largeText +=
            block;
    }


    const std::vector<std::string> basePatterns = {
        "quick",
        "brown",
        "lazy",
        "dog",
        "fox",
        "pattern_not_found",
        "computer_science",
        "algorithm_xyz",
        "data_structure",
        "missing_text"
    };


    std::vector<std::string> patterns;

    for (
        int i = 0;
        i < 100;
        ++i
    ) {
        for (
            const std::string& pattern :
            basePatterns
        ) {
            patterns.push_back(
                pattern
            );
        }
    }


    std::cout
        << "Benchmark configuration\n"
        << "-----------------------\n";

    std::cout
        << "Text size:        "
        << largeText.size()
        << " characters\n";

    std::cout
        << "Patterns searched:"
        << ' '
        << patterns.size()
        << '\n';

    std::cout
        << "Existing patterns:"
        << " 500\n";

    std::cout
        << "Missing patterns: "
        << "500\n\n";


    const std::vector<BenchmarkResult>
        results =
            runBenchmark(
                largeText,
                patterns
            );


    std::cout
        << std::left
        << std::setw(20)
        << "Algorithm"

        << std::setw(18)
        << "Time (ms)"

        << std::setw(18)
        << "Matches"

        << '\n';


    std::cout
        << std::string(
            56,
            '-'
        )
        << '\n';


    for (
        const BenchmarkResult& result :
        results
    ) {
        std::cout
            << std::left

            << std::setw(20)
            << result.algorithm

            << std::setw(18)
            << std::fixed
            << std::setprecision(3)
            << result.timeMilliseconds

            << std::setw(18)
            << result.matchesFound

            << '\n';
    }


    std::cout
        << "\nImportant:\n"
        << "The benchmark compares the algorithms using "
        << "the same text and the same patterns.\n"
        << "Execution time depends on the input data, "
        << "implementation and machine.\n";
}

void runCustomSearch() {
    printTitle(
        "CUSTOM PATTERN SEARCH"
    );

    std::cin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        '\n'
    );

    std::string text;
    std::string pattern;

    std::cout << "Enter text: ";
    std::getline(
        std::cin,
        text
    );

    std::cout << "Enter pattern: ";
    std::getline(
        std::cin,
        pattern
    );

    if (
        text.empty() ||
        pattern.empty()
    ) {
        std::cout
            << "\nText and pattern cannot be empty.\n";

        return;
    }

    if (
        pattern.size() >
        text.size()
    ) {
        std::cout
            << "\nPattern cannot be longer than text.\n";

        return;
    }


    std::cout
        << "\nChoose algorithm:\n"
        << "1. Brute Force\n"
        << "2. Boyer-Moore\n"
        << "3. KMP\n"
        << "4. Rabin-Karp\n"
        << "\nOption: ";


    int algorithmOption;

    std::cin >>
        algorithmOption;


    SearchResult result;

    std::string algorithmName;


    switch (
        algorithmOption
    ) {

        case 1:

            algorithmName =
                "Brute Force";

            result =
                bruteForceSearch(
                    text,
                    pattern,
                    true
                );

            break;


        case 2:

            algorithmName =
                "Boyer-Moore";

            result =
                boyerMooreSearch(
                    text,
                    pattern,
                    true
                );

            break;


        case 3:

            algorithmName =
                "Knuth-Morris-Pratt";

            result =
                kmpSearch(
                    text,
                    pattern,
                    true
                );

            break;


        case 4:

            algorithmName =
                "Rabin-Karp";

            result =
                rabinKarpSearch(
                    text,
                    pattern,
                    true
                );

            break;


        default:

            std::cout
                << "\nInvalid algorithm option.\n";

            return;
    }


    printDemoResult(
        algorithmName,
        text,
        pattern,
        result
    );


    if (
        algorithmOption == 2
    ) {
        printLastOccurrenceTable(
            pattern
        );
    }


    if (
        algorithmOption == 3
    ) {
        printFailureFunction(
            pattern
        );
    }


    if (
        algorithmOption == 4
    ) {
        std::cout
            << "\nPattern hash: "
            << computeHash(pattern)
            << '\n';
    }


    printSearchTrace(
        result
    );
}

// MENU

void printMenu() {
    printTitle(
        "PATTERN MATCHING PROJECT"
    );

    std::cout
        << "1. Algorithm demonstrations\n"
        << "2. Correctness tests\n"
        << "3. Performance benchmark\n"
        << "4. Run everything\n"
        << "5. Custom search\n"
        << "0. Exit\n\n"
        << "Select an option: ";
}

// MAIN

int main() {
    int option = -1;

    while (option != 0) {
        printMenu();

        std::cin >> option;

        switch (option) {

            case 1:
                runAlgorithmDemos();
                break;

            case 2:
                runAllTests();
                break;

            case 3:
                runBenchmarkDemo();
                break;

            case 4:
                runAlgorithmDemos();
                runAllTests();
                runBenchmarkDemo();
                break;
            
            case 5:
                runCustomSearch();
                break;

            case 0:
                std::cout
                    << "\nProgram finished.\n";
                break;

            default:
                std::cout
                    << "\nInvalid option.\n";
                break;
        }

        if (option != 0) {
            std::cout
                << "\nPress ENTER to return to the menu...";

            std::cin.ignore();
            std::cin.get();
        }
    }

    return 0;
}