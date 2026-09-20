# Pattern Matching Project

Proyecto académico de implementación y visualización de algoritmos de Pattern Matching en C++.
Grupo 4 :
- ALVARO MATIAS SALAZAR ZAPATA       - 24200205
- ALEXIS ORLANDO GONZALES HERNADEZ   - 24200201
- DIEGO ALEJANDRO SOTELO ATUNCAR     - 24200208
- PAOLO ALONSO VILLAVICENCIO MERELLA - 24200210

El proyecto incluye cuatro algoritmos principales:

- Brute Force : Compares the pattern with every possible position in the text. When a mismatch
                occurs, the pattern moves one position to the right. Worst-case complexity: O(nm)

- Boyer-Moore : Compares the pattern from right to left. This implementation uses the 
                bad-character rule and the Last Occurrence Table: L(x) The table stores the last
                position where each character appears in the pattern. This allows the algorithm to perform 
                larger shifts after a mismatch.

- Knuth-Morris-Pratt (KMP): Compares from left to right and avoids moving backwards in the text.
                            It preprocesses the pattern using the Failure Function: F(k)
                            This function stores the length of the largest prefix that is also a suffix.
                            When a mismatch occurs, KMP reuses previous matches instead of restarting the comparison.
                            Time complexity: O(n + m)

- Rabin-Karp: Rabin-Karp compares hashes instead of comparing all characters immediately.
              For each text window: hash(window) is compared with: hash(pattern)
              If both hashes match, the characters are verified.
              A matching hash is only considered a candidate because hash collisions may occur.
              This implementation uses a rolling hash.


También incluye pruebas de corrección, benchmark de rendimiento, trazas de ejecución y un visualizador web interactivo.

- Requirements:
    CMake 3.10 or higher
    C++17 compatible compiler
    Modern web browser
    Python 3 (optional, for running the visualizer with a local HTTP server)

- Build: 
    mkdir -p build
    cd build
    cmake ..
    cmake --build .
- Run : ./app_pattern_matching.exe
- Visualizer: 
    cd apps/visualizer
    python -m http.server 8000
    open : http://localhost:8000
---

## Project Structure

```text
pattern_matching/
├── apps/
│   ├── app_main.cpp
│   └── visualizer/
│       ├── index.html
│       ├── style.css
│       └── script.js
│
├── include/
│   ├── pattern_matching.h
│   ├── brute_force.h
│   ├── boyer_moore.h
│   ├── kmp.h
│   ├── rabin_karp.h
│   └── benchmark.h
│
├── src/
│   ├── brute_force.cpp
│   ├── boyer_moore.cpp
│   ├── kmp.cpp
│   ├── rabin_karp.cpp
│   └── benchmark.cpp
│
├── CMakeLists.txt
├── README.md
└── .gitignore