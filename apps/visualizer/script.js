// ============================================================
// DOM ELEMENTS
// ============================================================

const textInput =
    document.getElementById("textInput");

const patternInput =
    document.getElementById("patternInput");

const modeSelect =
    document.getElementById("modeSelect");

const algorithmSelect =
    document.getElementById("algorithmSelect");

const textRow =
    document.getElementById("textRow");

const patternRow =
    document.getElementById("patternRow");

const textTitle =
    document.getElementById("textTitle");

const eventText =
    document.getElementById("eventText");

const stepCounter =
    document.getElementById("stepCounter");

const comparisonCounter =
    document.getElementById("comparisonCounter");

const algorithmData =
    document.getElementById("algorithmData");


// ============================================================
// VISUALIZER STATE
// ============================================================

let steps = [];

let currentStep = -1;

let comparisonCount = 0;

let autoTimer = null;


// ============================================================
// GENERAL FUNCTIONS
// ============================================================

function createCharacterRow(
    container,
    value
) {
    container.innerHTML = "";

    for (
        let i = 0;
        i < value.length;
        ++i
    ) {
        const element =
            document.createElement("div");

        element.className =
            "character";

        element.textContent =
            value[i];

        element.dataset.index =
            i;

        container.appendChild(
            element
        );
    }
}


function clearHighlights() {
    document
        .querySelectorAll(
            ".character"
        )
        .forEach(
            (element) => {

                element.classList.remove(
                    "match",
                    "mismatch",
                    "active"
                );
            }
        );
}


function updateCounters() {
    const visibleStep =
        currentStep >= 0
            ? currentStep + 1
            : 0;

    stepCounter.textContent =
        `${visibleStep} / ${steps.length}`;

    comparisonCount =
        currentStep >= 0
            ? steps
                .slice(
                    0,
                    currentStep + 1
                )
                .filter(
                    (step) =>
                        step.type ===
                        "COMPARE"
                )
                .length
            : 0;

    comparisonCounter.textContent =
        comparisonCount;
}


// ============================================================
// BIOINFORMATICS
// ============================================================

function isValidDNA(value) {
    return /^[ACGT]+$/i.test(value);
}


function loadExample() {
    stopAuto();

    if (
        modeSelect.value ===
        "dna"
    ) {
        textInput.value =
            "ACGTACGGTACGTAC";

        patternInput.value =
            "CGGTA";

        textTitle.textContent =
            "DNA Sequence";
    }
    else {
        textInput.value =
            "ababaca";

        patternInput.value =
            "abaca";

        textTitle.textContent =
            "Text";
    }

    algorithmSelect.value =
        "kmp";

    resetVisualization();
}


// ============================================================
// BRUTE FORCE
// ============================================================

function bruteForceSteps(
    text,
    pattern
) {
    const result = [];

    const n = text.length;
    const m = pattern.length;

    for (
        let alignment = 0;
        alignment <= n - m;
        ++alignment
    ) {
        let patternIndex = 0;

        while (
            patternIndex < m
        ) {
            const textIndex =
                alignment +
                patternIndex;

            const matches =
                text[textIndex] ===
                pattern[patternIndex];

            result.push({
                type:
                    "COMPARE",

                textIndex,

                patternIndex,

                alignment,

                matches,

                message:
                    matches
                        ? "Characters match"
                        : "Characters do not match"
            });

            if (!matches) {
                break;
            }

            ++patternIndex;
        }

        if (
            patternIndex === m
        ) {
            result.push({
                type:
                    "FOUND",

                alignment,

                message:
                    "Pattern found"
            });

            return result;
        }

        if (
            alignment <
            n - m
        ) {
            result.push({
                type:
                    "SHIFT",

                alignment,

                shift:
                    1,

                message:
                    "Brute Force shifts the pattern by one position"
            });
        }
    }

    return result;
}


// ============================================================
// BOYER-MOORE
// ============================================================

function buildLastOccurrence(
    pattern
) {
    const table = {};

    for (
        let i = 0;
        i < pattern.length;
        ++i
    ) {
        table[
            pattern[i]
        ] = i;
    }

    return table;
}


function boyerMooreSteps(
    text,
    pattern
) {
    const result = [];

    const n = text.length;
    const m = pattern.length;

    const lastOccurrence =
        buildLastOccurrence(
            pattern
        );

    let alignment = 0;

    while (
        alignment <=
        n - m
    ) {
        let patternIndex =
            m - 1;

        while (
            patternIndex >= 0
        ) {
            const textIndex =
                alignment +
                patternIndex;

            const matches =
                pattern[
                    patternIndex
                ] ===
                text[
                    textIndex
                ];

            result.push({
                type:
                    "COMPARE",

                textIndex,

                patternIndex,

                alignment,

                matches,

                message:
                    matches
                        ? "Characters match"
                        : "Characters do not match"
            });

            if (!matches) {
                break;
            }

            --patternIndex;
        }

        if (
            patternIndex < 0
        ) {
            result.push({
                type:
                    "FOUND",

                alignment,

                message:
                    "Pattern found"
            });

            return {
                steps:
                    result,

                table:
                    lastOccurrence
            };
        }

        const badCharacter =
            text[
                alignment +
                patternIndex
            ];

        const lastIndex =
            lastOccurrence[
                badCharacter
            ] ?? -1;

        const shift =
            Math.max(
                1,
                patternIndex -
                lastIndex
            );

        result.push({
            type:
                "SHIFT",

            textIndex:
                alignment +
                patternIndex,

            patternIndex,

            alignment,

            shift,

            message:
                `Bad-character '${badCharacter}' causes a shift of ${shift}`
        });

        alignment +=
            shift;
    }

    return {
        steps:
            result,

        table:
            lastOccurrence
    };
}


// ============================================================
// KMP
// ============================================================

function buildFailureFunction(
    pattern
) {
    const failure =
        new Array(
            pattern.length
        ).fill(0);

    let prefixLength =
        0;

    let index =
        1;

    while (
        index <
        pattern.length
    ) {
        if (
            pattern[index] ===
            pattern[prefixLength]
        ) {
            ++prefixLength;

            failure[index] =
                prefixLength;

            ++index;
        }

        else if (
            prefixLength > 0
        ) {
            prefixLength =
                failure[
                    prefixLength - 1
                ];
        }

        else {
            failure[index] =
                0;

            ++index;
        }
    }

    return failure;
}


function kmpSteps(
    text,
    pattern
) {
    const result = [];

    const failure =
        buildFailureFunction(
            pattern
        );

    let textIndex =
        0;

    let patternIndex =
        0;

    while (
        textIndex <
        text.length
    ) {
        const matches =
            text[
                textIndex
            ] ===
            pattern[
                patternIndex
            ];

        result.push({
            type:
                "COMPARE",

            textIndex,

            patternIndex,

            alignment:
                textIndex -
                patternIndex,

            matches,

            message:
                matches
                    ? "Characters match"
                    : "Characters do not match"
        });

        if (matches) {
            ++textIndex;

            ++patternIndex;

            if (
                patternIndex ===
                pattern.length
            ) {
                result.push({
                    type:
                        "FOUND",

                    alignment:
                        textIndex -
                        pattern.length,

                    message:
                        "Pattern found"
                });

                return {
                    steps:
                        result,

                    failure
                };
            }
        }

        else if (
            patternIndex > 0
        ) {
            const previousIndex =
                patternIndex - 1;

            const newPatternIndex =
                failure[
                    previousIndex
                ];

            result.push({
                type:
                    "FALLBACK",

                textIndex,

                patternIndex,

                alignment:
                    textIndex -
                    patternIndex,

                shift:
                    patternIndex -
                    newPatternIndex,

                nextPatternIndex:
                    newPatternIndex,

                failureIndex:
                    previousIndex,

                failureValue:
                    newPatternIndex,

                message:
                    `Mismatch: F(${previousIndex}) = ${newPatternIndex}. Continue at P[${newPatternIndex}]`
            });

            patternIndex =
                newPatternIndex;
        }

        else {
            result.push({
                type:
                    "SHIFT",

                textIndex,

                patternIndex,

                alignment:
                    textIndex,

                shift:
                    1,

                message:
                    "No reusable prefix. Advance in the text."
            });

            ++textIndex;
        }
    }

    return {
        steps:
            result,

        failure
    };
}


// ============================================================
// RABIN-KARP
// ============================================================

const HASH_BASE =
    256;

const HASH_MOD =
    1000000007;


function computeHash(
    value
) {
    let hash = 0;

    for (
        const character
        of value
    ) {
        hash =
            (
                hash *
                HASH_BASE +
                character.charCodeAt(0)
            )
            %
            HASH_MOD;
    }

    return hash;
}


function rabinKarpSteps(
    text,
    pattern
) {
    const result = [];

    const n =
        text.length;

    const m =
        pattern.length;

    let highestPower =
        1;

    for (
        let i = 0;
        i < m - 1;
        ++i
    ) {
        highestPower =
            (
                highestPower *
                HASH_BASE
            )
            %
            HASH_MOD;
    }

    const patternHash =
        computeHash(
            pattern
        );

    let windowHash =
        computeHash(
            text.substring(
                0,
                m
            )
        );

    for (
        let alignment = 0;
        alignment <= n - m;
        ++alignment
    ) {
        const hashMatches =
            windowHash ===
            patternHash;

        result.push({
            type:
                "HASH_CHECK",

            alignment,

            textHash:
                windowHash,

            patternHash,

            matches:
                hashMatches,

            message:
                `Window hash = ${windowHash} | Pattern hash = ${patternHash}`
        });

        if (
            hashMatches
        ) {
            result.push({
                type:
                    "CANDIDATE",

                alignment,

                message:
                    "Hashes match. Verify the characters."
            });

            let fullMatch =
                true;

            for (
                let patternIndex = 0;
                patternIndex < m;
                ++patternIndex
            ) {
                const textIndex =
                    alignment +
                    patternIndex;

                const matches =
                    text[
                        textIndex
                    ] ===
                    pattern[
                        patternIndex
                    ];

                result.push({
                    type:
                        "COMPARE",

                    textIndex,

                    patternIndex,

                    alignment,

                    matches,

                    message:
                        matches
                            ? "Candidate character matches"
                            : "Candidate character does not match"
                });

                if (
                    !matches
                ) {
                    fullMatch =
                        false;

                    result.push({
                        type:
                            "COLLISION",

                        alignment,

                        message:
                            "Same hash but different characters: collision"
                    });

                    break;
                }
            }

            if (
                fullMatch
            ) {
                result.push({
                    type:
                        "FOUND",

                    alignment,

                    message:
                        "Pattern found"
                });

                return result;
            }
        }

        if (
            alignment <
            n - m
        ) {
            result.push({
                type:
                    "SHIFT",

                alignment,

                shift:
                    1,

                message:
                    "Move the rolling hash window one position"
            });

            const outgoingCharacter =
                text.charCodeAt(
                    alignment
                );

            const incomingCharacter =
                text.charCodeAt(
                    alignment + m
                );

            windowHash =
                (
                    windowHash -
                    (
                        outgoingCharacter *
                        highestPower
                    )
                )
                %
                HASH_MOD;

            if (
                windowHash <
                0
            ) {
                windowHash +=
                    HASH_MOD;
            }

            windowHash =
                (
                    windowHash *
                    HASH_BASE +
                    incomingCharacter
                )
                %
                HASH_MOD;
        }
    }

    return result;
}


// ============================================================
// ALGORITHM INFORMATION
// ============================================================

function renderLastOccurrence(
    table
) {
    algorithmData.innerHTML =
        "<h3>Last Occurrence Table L(x)</h3>";

    const container =
        document.createElement(
            "div"
        );

    container.className =
        "data-array";

    for (
        const [
            character,
            index
        ]
        of Object.entries(
            table
        )
    ) {
        const element =
            document.createElement(
                "div"
            );

        element.className =
            "data-item";

        element.textContent =
            `L(${character}) = ${index}`;

        container.appendChild(
            element
        );
    }

    algorithmData.appendChild(
        container
    );
}


function renderFailureFunction(
    failure
) {
    algorithmData.innerHTML =
        "<h3>Failure Function F(k)</h3>";

    const container =
        document.createElement(
            "div"
        );

    container.className =
        "data-array";

    failure.forEach(
        (
            value,
            index
        ) => {

            const element =
                document.createElement(
                    "div"
                );

            element.className =
                "data-item";

            element.textContent =
                `F(${index}) = ${value}`;

            container.appendChild(
                element
            );
        }
    );

    algorithmData.appendChild(
        container
    );
}


function renderRabinKarpInfo(
    pattern
) {
    const patternHash =
        computeHash(
            pattern
        );

    algorithmData.innerHTML =
        `
        <h3>Rolling Hash</h3>

        <p>
            Pattern hash:
            <strong>${patternHash}</strong>
        </p>

        <p>
            A matching hash is only a candidate.
            The real characters are verified afterwards.
        </p>
        `;
}


// ============================================================
// START
// ============================================================

function startVisualization() {
    stopAuto();

    let text =
        textInput.value.trim();

    let pattern =
        patternInput.value.trim();

    const mode =
        modeSelect.value;


    if (
        text.length === 0 ||
        pattern.length === 0
    ) {
        alert(
            "Text and pattern cannot be empty."
        );

        return;
    }


    if (
        mode ===
        "dna"
    ) {
        text =
            text.toUpperCase();

        pattern =
            pattern.toUpperCase();

        textInput.value =
            text;

        patternInput.value =
            pattern;

        textTitle.textContent =
            "DNA Sequence";


        if (
            !isValidDNA(
                text
            )
        ) {
            alert(
                "DNA sequence can only contain A, C, G and T."
            );

            return;
        }


        if (
            !isValidDNA(
                pattern
            )
        ) {
            alert(
                "DNA pattern can only contain A, C, G and T."
            );

            return;
        }
    }
    else {
        textTitle.textContent =
            "Text";
    }


    if (
        pattern.length >
        text.length
    ) {
        alert(
            "Pattern cannot be longer than text."
        );

        return;
    }


    createCharacterRow(
        textRow,
        text
    );

    createCharacterRow(
        patternRow,
        pattern
    );


    patternRow.style.transform =
        "translateX(0px)";


    steps =
        [];

    currentStep =
        -1;

    comparisonCount =
        0;

    algorithmData.innerHTML =
        "";


    const algorithm =
        algorithmSelect.value;


    switch (
        algorithm
    ) {

        case "bruteForce":

            steps =
                bruteForceSteps(
                    text,
                    pattern
                );

            algorithmData.innerHTML =
                `
                <h3>Brute Force</h3>

                <p>
                    Every mismatch moves the
                    pattern one position.
                </p>
                `;

            break;


        case "boyerMoore": {

            const result =
                boyerMooreSteps(
                    text,
                    pattern
                );

            steps =
                result.steps;

            renderLastOccurrence(
                result.table
            );

            break;
        }


        case "kmp": {

            const result =
                kmpSteps(
                    text,
                    pattern
                );

            steps =
                result.steps;

            renderFailureFunction(
                result.failure
            );

            break;
        }


        case "rabinKarp":

            steps =
                rabinKarpSteps(
                    text,
                    pattern
                );

            renderRabinKarpInfo(
                pattern
            );

            break;
    }


    eventText.textContent =
        "Ready. Press Next or Auto.";

    updateCounters();
}


// ============================================================
// RENDER CURRENT STEP
// ============================================================

function renderStep() {
    if (
        currentStep < 0 ||
        currentStep >=
        steps.length
    ) {
        return;
    }


    clearHighlights();


    const step =
        steps[
            currentStep
        ];


    const characterWidth =
        46;


    const alignment =
        step.alignment ??
        0;


    patternRow.style.transform =
        `translateX(${alignment * characterWidth}px)`;


    if (
        step.type ===
        "COMPARE"
    ) {
        const textCharacter =
            textRow.children[
                step.textIndex
            ];

        const patternCharacter =
            patternRow.children[
                step.patternIndex
            ];


        const className =
            step.matches
                ? "match"
                : "mismatch";


        if (
            textCharacter
        ) {
            textCharacter.classList.add(
                className,
                "active"
            );
        }


        if (
            patternCharacter
        ) {
            patternCharacter.classList.add(
                className,
                "active"
            );
        }
    }


    if (
        step.type ===
        "HASH_CHECK" ||
        step.type ===
        "CANDIDATE"
    ) {
        const patternLength =
            patternInput.value.length;

        for (
            let i = 0;
            i < patternLength;
            ++i
        ) {
            const textCharacter =
                textRow.children[
                    alignment + i
                ];

            if (
                textCharacter
            ) {
                textCharacter.classList.add(
                    "active"
                );
            }
        }
    }


    if (
        step.type ===
        "FOUND"
    ) {
        const patternLength =
            patternInput.value.length;

        for (
            let i = 0;
            i < patternLength;
            ++i
        ) {
            const patternCharacter =
                patternRow.children[
                    i
                ];

            const textCharacter =
                textRow.children[
                    alignment + i
                ];

            if (
                patternCharacter
            ) {
                patternCharacter.classList.add(
                    "match"
                );
            }

            if (
                textCharacter
            ) {
                textCharacter.classList.add(
                    "match"
                );
            }
        }
    }


    eventText.textContent =
        `${step.type}: ${step.message}`;


    updateCounters();
}


// ============================================================
// NAVIGATION
// ============================================================

function nextStep() {
    if (
        steps.length ===
        0
    ) {
        return;
    }

    if (
        currentStep <
        steps.length - 1
    ) {
        ++currentStep;

        renderStep();
    }
}


function previousStep() {
    if (
        steps.length ===
        0
    ) {
        return;
    }


    if (
        currentStep >
        0
    ) {
        --currentStep;

        renderStep();
    }

    else if (
        currentStep ===
        0
    ) {
        currentStep =
            -1;

        clearHighlights();

        patternRow.style.transform =
            "translateX(0px)";

        eventText.textContent =
            "Ready. Press Next or Auto.";

        updateCounters();
    }
}


// ============================================================
// AUTO MODE
// ============================================================

function startAuto() {
    if (
        steps.length ===
        0
    ) {
        return;
    }

    stopAuto();

    autoTimer =
        setInterval(
            () => {

                if (
                    currentStep >=
                    steps.length - 1
                ) {
                    stopAuto();

                    return;
                }

                nextStep();

            },
            800
        );
}


function stopAuto() {
    if (
        autoTimer !==
        null
    ) {
        clearInterval(
            autoTimer
        );

        autoTimer =
            null;
    }
}


// ============================================================
// RESET
// ============================================================

function resetVisualization() {
    stopAuto();

    steps =
        [];

    currentStep =
        -1;

    comparisonCount =
        0;

    textRow.innerHTML =
        "";

    patternRow.innerHTML =
        "";

    patternRow.style.transform =
        "translateX(0px)";

    algorithmData.innerHTML =
        "";

    eventText.textContent =
        "Press Start to begin.";

    updateCounters();
}


// ============================================================
// EVENTS
// ============================================================

document
    .getElementById(
        "startButton"
    )
    .addEventListener(
        "click",
        startVisualization
    );


document
    .getElementById(
        "nextButton"
    )
    .addEventListener(
        "click",
        nextStep
    );


document
    .getElementById(
        "previousButton"
    )
    .addEventListener(
        "click",
        previousStep
    );


document
    .getElementById(
        "autoButton"
    )
    .addEventListener(
        "click",
        startAuto
    );


document
    .getElementById(
        "resetButton"
    )
    .addEventListener(
        "click",
        resetVisualization
    );


document
    .getElementById(
        "exampleButton"
    )
    .addEventListener(
        "click",
        loadExample
    );


modeSelect.addEventListener(
    "change",
    loadExample
);