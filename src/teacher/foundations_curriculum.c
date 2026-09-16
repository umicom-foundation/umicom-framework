/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/teacher/foundations_curriculum.c
 *
 * PURPOSE:
 *   Publish one human-readable beginner curriculum for Studio, the command
 *   line, web learning tools and future Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/foundations_curriculum.h"

#include <limits.h>
#include <string.h>

/* Lesson identity is persistent state. Keep the established identifiers even
 * where the public title now teaches the main-only contribution workflow.
 * resource_path remains relative to the Applications source root, as consumed
 * by the existing Studio documentation surface. Standalone reading is through
 * docs/learning/index.html in Framework or the installed learning directory. */
static const UmiTeacherFoundationsLesson FOUNDATIONS_LESSONS[] = {
    {
        "foundations.welcome", "Welcome to building software",
        "Learn what a program, source file, compiler, repository and application are.",
        "Match five everyday explanations to the correct software words.",
        "framework/docs/learning/welcome.html", UMI_TEACHER_FOUNDATIONS_ORIENTATION,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_FOUNDATION,
        1U, 20U, 70U, 0, 0, 0
    },
    {
        "foundations.files", "Folders, files and safe paths",
        "Create a practice folder, recognise a file extension and use an absolute path.",
        "Create a folder named umicom-practice and explain where it lives.",
        "framework/docs/learning/files-and-folders.html", UMI_TEACHER_FOUNDATIONS_ORIENTATION,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_FOUNDATION,
        2U, 25U, 70U, 0, 0, 0
    },
    {
        "foundations.terminal", "Your first terminal commands",
        "Open PowerShell or a Linux terminal, change folder and list its contents.",
        "Move into the practice folder, list it, then return to its parent.",
        "framework/docs/learning/terminal.html", UMI_TEACHER_FOUNDATIONS_TOOLS,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_FOUNDATION,
        3U, 30U, 70U, 0, 0, 0
    },
    {
        "foundations.toolchain", "Check the development tools",
        "Check a C compiler, Git, CMake and Ninja before creating a build.",
        "Run each version command and identify the tool that reports a missing program.",
        "framework/docs/learning/development-tools.html", UMI_TEACHER_FOUNDATIONS_TOOLS,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_FOUNDATION,
        4U, 35U, 75U, 0, 0, 0
    },
    {
        "foundations.c-hello", "A tiny C program",
        "Read main, include a header, print a message and return a success code.",
        "Create, compile and run a program that prints Hello from Umicom.",
        "framework/docs/learning/first-c-program.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_FOUNDATION,
        5U, 45U, 75U, 1, 0, 0
    },
    {
        "foundations.c-data", "Names, values and decisions in C",
        "Use meaningful variables, basic data types, if statements and loops.",
        "Write a loop that labels scores as ready or needs more practice.",
        "framework/docs/learning/values-and-decisions.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        6U, 60U, 75U, 1, 0, 0
    },
    {
        "foundations.c-functions", "Functions and clear contracts",
        "Split work into small functions with inputs, outputs and useful names.",
        "Extract score validation into a function and test three inputs.",
        "framework/docs/learning/functions.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        7U, 60U, 75U, 1, 0, 0
    },
    {
        "foundations.c-memory", "Pointers, arrays and memory safety",
        "Understand addresses, NULL, array bounds, ownership and cleanup.",
        "Find and repair three deliberately unsafe pointer examples.",
        "framework/docs/learning/arrays-and-pointers.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        8U, 75U, 80U, 1, 0, 0
    },
    {
        "foundations.c-modules", "Headers, source files and tests",
        "Create a small public contract, keep implementation private and verify behavior.",
        "Build one header, one source file and one focused test without warnings.",
        "framework/docs/learning/headers-and-tests.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        9U, 75U, 80U, 1, 0, 0
    },
    {
        "foundations.git", "Git without mystery",
        "Learn working tree, staging area, commit, branch, remote and push.",
        "Make a practice change, inspect it, stage it and create a local commit.",
        "framework/docs/learning/git-first-commit.html", UMI_TEACHER_FOUNDATIONS_TOOLS,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_BEGINNER,
        10U, 60U, 75U, 0, 1, 0
    },
    {
        "foundations.clone", "Clone Umicom and its submodules",
        "Clone the suite and read the exact revisions recorded for Framework and the applications.",
        "Clone with Git, initialise submodules and compare their recorded and checked-out revisions.",
        "framework/docs/learning/clone-and-update.html", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_BEGINNER,
        11U, 45U, 80U, 0, 1, 0
    },
    {
        "foundations.cmake", "Build with CMake and Ninja",
        "Separate source, configure, compile, test and install steps.",
        "Build the lesson examples in a new build directory and run their tests.",
        "framework/docs/learning/cmake.html", UMI_TEACHER_FOUNDATIONS_TOOLS,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        12U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.framework", "How Umicom Framework fits together",
        "Follow a public C contract through its implementation, tests and consuming application.",
        "Find the shared window constructor and explain which repository owns it.",
        "framework/docs/learning/framework-architecture.html", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        13U, 75U, 80U, 1, 1, 0
    },
    {
        "foundations.gtk", "GTK widgets and events",
        "Read the event loop, the widget tree and a button callback.",
        "Trace a click from the signal connection to the displayed message.",
        "framework/docs/learning/gtk-events.html", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        14U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.components", "Build a window with Framework components",
        "Use Framework component specifications and GTK adapters to compose a working window.",
        "Build the learning window, click its button, resize it and close it cleanly.",
        "framework/docs/learning/framework-window.html", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        15U, 75U, 80U, 1, 1, 0
    },
    {
        "foundations.change", "Make one quality Framework change",
        "Make one small, reviewable change with a focused test and a clear explanation.",
        "Change the learning window message without changing the shared component implementation.",
        "framework/docs/learning/first-framework-change.html", UMI_TEACHER_FOUNDATIONS_CONTRIBUTION,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        16U, 120U, 85U, 1, 1, 0
    },
    {
        "foundations.branch", "Work safely on main",
        "Inspect the current branch, preserve detached work and make a local checkpoint.",
        "Confirm main before editing; review the staged diff without discarding local work.",
        "framework/docs/learning/working-on-main.html", UMI_TEACHER_FOUNDATIONS_CONTRIBUTION,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_BEGINNER,
        17U, 45U, 80U, 0, 1, 0
    },
    {
        "foundations.pull-request", "Publish a change and update Linux",
        "Push a reviewed commit, record child revisions in the parent and update a second checkout.",
        "Publish to a repository you own, then pull and test its recorded revisions in Linux.",
        "framework/docs/learning/publish-and-pull.html", UMI_TEACHER_FOUNDATIONS_CONTRIBUTION,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_BEGINNER,
        18U, 60U, 85U, 0, 1, 1
    },
    {
        "foundations.bits", "Bits, bytes and hexadecimal",
        "Read small unsigned values in binary, decimal and hexadecimal.",
        "Print the same value in three forms and explain its individual bits.",
        "framework/docs/learning/bits-and-bytes.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_FOUNDATION,
        19U, 45U, 75U, 1, 0, 0
    },
    {
        "foundations.assembly", "Read and call Assembly",
        "Compare C with compiler output and call a small architecture-specific function.",
        "Build the C and Assembly comparison, then explain the argument and return registers.",
        "framework/docs/learning/assembly.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_ASSEMBLY, UMI_TEACHER_LEVEL_BEGINNER,
        20U, 90U, 80U, 1, 0, 0
    },
    {
        "foundations.git-workflow", "Git from clone to publication",
        "Practise Git on main, publish child repositories first and reproduce the parent snapshot on another machine.",
        "Create and modify a file in a disposable Git repository; add, commit, push, clone and verify the second checkout.",
        "framework/docs/learning/git-workflow.html", UMI_TEACHER_FOUNDATIONS_CONTRIBUTION,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_BEGINNER,
        21U, 180U, 80U, 0, 1, 1
    },
    {
        "foundations.binary-arithmetic", "Binary arithmetic from first principles",
        "A bit is a position with two possible values.",
        "Build umicom-learning-binary-arithmetic; run its checks, change one input and explain the result.",
        "framework/docs/learning/binary-arithmetic.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        22U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.boolean-masks", "Boolean decisions and bit masks",
        "A Boolean decision answers yes or no.",
        "Build umicom-learning-boolean-masks; run its checks, change one input and explain the result.",
        "framework/docs/learning/boolean-masks.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        23U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.integer-ranges", "Integer ranges and checked conversions",
        "A type has a range.",
        "Build umicom-learning-integer-ranges; run its checks, change one input and explain the result.",
        "framework/docs/learning/integer-ranges.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        24U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.byte-order", "Bytes, endianness and explicit encoding",
        "An integer value and its byte representation are different things.",
        "Build umicom-learning-byte-order; run its checks, change one input and explain the result.",
        "framework/docs/learning/byte-order.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        25U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.cpu-model", "A tiny fetch-decode-execute machine",
        "A CPU fetches an instruction, decodes its meaning and changes state.",
        "Build umicom-learning-cpu-model; run its checks, change one input and explain the result.",
        "framework/docs/learning/cpu-model.html", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        26U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.memory-lifetimes", "Object lifetime, stack and heap",
        "A pointer is usable only while the object it names is alive.",
        "Build umicom-learning-memory-lifetimes; run its checks, change one input and explain the result.",
        "framework/docs/learning/memory-lifetimes.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        27U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.types-formatting", "C values, types and output formats",
        "A value has a type, and printf needs a format matching the argument after default promotions.",
        "Build umicom-learning-types-formatting; run its checks, change one input and explain the result.",
        "framework/docs/learning/types-formatting.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        28U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.control-flow", "Decisions, loops and early returns",
        "Control flow determines which statement executes next.",
        "Build umicom-learning-control-flow; run its checks, change one input and explain the result.",
        "framework/docs/learning/control-flow.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        29U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.functions-contracts", "Functions with explicit contracts",
        "A function should state valid input, ownership, result and failure behaviour.",
        "Build umicom-learning-functions-contracts; run its checks, change one input and explain the result.",
        "framework/docs/learning/functions-contracts.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        30U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.array-bounds", "Array boundaries and empty collections",
        "An array of N elements has indexes 0 through N minus one.",
        "Build umicom-learning-array-bounds; run its checks, change one input and explain the result.",
        "framework/docs/learning/array-bounds.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        31U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.bounded-strings", "Bounded text and null termination",
        "A C string is terminated by a zero byte.",
        "Build umicom-learning-bounded-strings; run its checks, change one input and explain the result.",
        "framework/docs/learning/bounded-strings.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        32U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.numeric-parsing", "Parsing numbers without accepting junk",
        "Text input must satisfy a syntax and a range.",
        "Build umicom-learning-numeric-parsing; run its checks, change one input and explain the result.",
        "framework/docs/learning/numeric-parsing.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        33U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.records-enums", "Structures, enumerations and state",
        "A structure groups related fields.",
        "Build umicom-learning-records-enums; run its checks, change one input and explain the result.",
        "framework/docs/learning/records-enums.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        34U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.pointer-contracts", "Pointers, borrowing and const",
        "A pointer permits access to an object; it does not automatically convey ownership, lifetime or array length.",
        "Build umicom-learning-pointer-contracts; run its checks, change one input and explain the result.",
        "framework/docs/learning/pointer-contracts.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        35U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.dynamic-memory", "Allocation, resizing and ownership",
        "Dynamic memory is useful when sizes are chosen at runtime.",
        "Build umicom-learning-dynamic-memory; run its checks, change one input and explain the result.",
        "framework/docs/learning/dynamic-memory.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        36U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.checked-arithmetic", "Check arithmetic before overflow",
        "Unsigned arithmetic wraps modulo its range; signed overflow is undefined behaviour.",
        "Build umicom-learning-checked-arithmetic; run its checks, change one input and explain the result.",
        "framework/docs/learning/checked-arithmetic.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        37U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.finite-numbers", "Floating-point values and finite input",
        "Floating-point calculations are approximate and may involve NaN or infinities on the supported hosts.",
        "Build umicom-learning-finite-numbers; run its checks, change one input and explain the result.",
        "framework/docs/learning/finite-numbers.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        38U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.resource-cleanup", "Files, errors and one cleanup path",
        "Files and memory both have lifetimes.",
        "Build umicom-learning-resource-cleanup; run its checks, change one input and explain the result.",
        "framework/docs/learning/resource-cleanup.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        39U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.modules-build", "Headers, linking and separate compilation",
        "A header declares a contract; a source file defines it.",
        "Build umicom-learning-modules-build; run its checks, change one input and explain the result.",
        "framework/docs/learning/modules-build.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        40U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.callbacks", "Callbacks and dependency injection",
        "A callback supplies behaviour through a function pointer.",
        "Build umicom-learning-callbacks; run its checks, change one input and explain the result.",
        "framework/docs/learning/callbacks.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        41U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.controller-architecture", "Master Controller and Slave Controllers",
        "The Master Controller coordinates work.",
        "Build umicom-learning-controller-architecture; run its checks, change one input and explain the result.",
        "framework/docs/learning/controller-architecture.html", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        42U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.state-machines", "State machines and rejected transitions",
        "A state machine lists legal transitions rather than accepting arbitrary field assignments.",
        "Build umicom-learning-state-machines; run its checks, change one input and explain the result.",
        "framework/docs/learning/state-machines.html", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        43U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.bounded-queue", "A bounded queue and backpressure",
        "A queue preserves first-in, first-out order.",
        "Build umicom-learning-bounded-queue; run its checks, change one input and explain the result.",
        "framework/docs/learning/bounded-queue.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        44U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.search-sort", "Search, sorting and algorithm cost",
        "An algorithm is a method for solving a problem.",
        "Build umicom-learning-search-sort; run its checks, change one input and explain the result.",
        "framework/docs/learning/search-sort.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        45U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.test-design", "Tests that remain active in Release",
        "A test must actually execute its checks in every configuration it claims to validate.",
        "Build umicom-learning-test-design; run its checks, change one input and explain the result.",
        "framework/docs/learning/test-design.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        46U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.coding-conventions", "Readable names and coding conventions",
        "Conventions make intent consistent; they do not replace contracts or tests.",
        "Build umicom-learning-coding-conventions; run its checks, change one input and explain the result.",
        "framework/docs/learning/coding-conventions.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        47U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.macro-hazards", "Macros, precedence and repeated evaluation",
        "The preprocessor performs token substitution before C compilation.",
        "Build umicom-learning-macro-hazards; run its checks, change one input and explain the result.",
        "framework/docs/learning/macro-hazards.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        48U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.binary-records", "Serialisation without dumping structures",
        "A durable record needs a specified format.",
        "Build umicom-learning-binary-records; run its checks, change one input and explain the result.",
        "framework/docs/learning/binary-records.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        49U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.recursion", "Recursion with a bound and base case",
        "A recursive function calls itself on a smaller problem.",
        "Build umicom-learning-recursion; run its checks, change one input and explain the result.",
        "framework/docs/learning/recursion.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        50U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.atomic-basics", "Atomic operations and concurrency boundaries",
        "Concurrent access needs a documented rule.",
        "Build umicom-learning-atomic-basics; run its checks, change one input and explain the result.",
        "framework/docs/learning/atomic-basics.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        51U, 60U, 80U, 1, 0, 0
    },
    {
        "foundations.assembly-branches", "Assembly registers, comparisons and branches",
        "Assembly exposes register-level operations and control flow.",
        "Build umicom-learning-assembly-branches; run its checks, change one input and explain the result.",
        "framework/docs/learning/assembly-branches.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_ASSEMBLY, UMI_TEACHER_LEVEL_BEGINNER,
        52U, 90U, 80U, 1, 0, 0
    },
    {
        "foundations.assembly-loops", "Assembly loops, memory and empty input",
        "A memory loop must handle zero length before loading an element.",
        "Build umicom-learning-assembly-loops; run its checks, change one input and explain the result.",
        "framework/docs/learning/assembly-loops.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_ASSEMBLY, UMI_TEACHER_LEVEL_BEGINNER,
        53U, 90U, 80U, 1, 0, 0
    },
};

/* Appending lessons must not silently exceed the existing bounded plan. */
_Static_assert(sizeof(FOUNDATIONS_LESSONS) / sizeof(FOUNDATIONS_LESSONS[0]) <= UMI_TEACHER_MEDIUM_CAPACITY,
               "Foundations curriculum exceeds learning-plan capacity");

/*
 * Return the number of records represented by teacher foundations curriculum without
 * changing their state.
 */
size_t umi_teacher_foundations_curriculum_count(void)
{
    return sizeof(FOUNDATIONS_LESSONS) / sizeof(FOUNDATIONS_LESSONS[0]);
}

/*
 * Find teacher foundations curriculum while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTeacherFoundationsLesson *umi_teacher_foundations_curriculum_at(
    size_t index)
{
    return index < umi_teacher_foundations_curriculum_count()
        ? &FOUNDATIONS_LESSONS[index] : NULL;
}

/*
 * Find teacher foundations curriculum while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiTeacherFoundationsLesson *umi_teacher_foundations_curriculum_find(
    const char *lesson_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lesson_id == NULL || lesson_id[0] == '\0') return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_teacher_foundations_curriculum_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(FOUNDATIONS_LESSONS[index].id, lesson_id) == 0) {
            return &FOUNDATIONS_LESSONS[index];
        }
    }
    return NULL;
}

/*
 * Provide the teacher foundations curriculum next operation used by this module and its
 * client applications.
 */
const UmiTeacherFoundationsLesson *umi_teacher_foundations_curriculum_next(
    const char *lesson_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (lesson_id == NULL || lesson_id[0] == '\0') {
        return umi_teacher_foundations_curriculum_at(0U);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_teacher_foundations_curriculum_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(FOUNDATIONS_LESSONS[index].id, lesson_id) == 0) {
            return umi_teacher_foundations_curriculum_at(index + 1U);
        }
    }
    return NULL;
}

/*
 * Provide the teacher foundations curriculum plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_teacher_foundations_curriculum_plan(
    UmiTeacherLearningPlan *out_plan)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_teacher_learning_plan_init(out_plan);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_teacher_foundations_curriculum_count(); ++index) {
        status = umi_teacher_learning_plan_append(
            out_plan, FOUNDATIONS_LESSONS[index].id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the teacher foundations curriculum minutes operation used by this module and its
 * client applications.
 */
uint32_t umi_teacher_foundations_curriculum_minutes(void)
{
    size_t index;
    uint32_t total = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_teacher_foundations_curriculum_count(); ++index) {
        uint32_t minutes = FOUNDATIONS_LESSONS[index].estimated_minutes;
        /* Apply this branch only when its contract condition is satisfied. */
        if (UINT32_MAX - total < minutes) return UINT32_MAX;
        total += minutes;
    }
    return total;
}

/*
 * Provide the teacher foundations stage text operation used by this module and its client
 * applications.
 */
const char *umi_teacher_foundations_stage_text(
    UmiTeacherFoundationsStage stage)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (stage) {
        case UMI_TEACHER_FOUNDATIONS_ORIENTATION: return "orientation";
        case UMI_TEACHER_FOUNDATIONS_TOOLS: return "development tools";
        case UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING: return "C programming";
        case UMI_TEACHER_FOUNDATIONS_FRAMEWORK: return "Umicom Framework";
        case UMI_TEACHER_FOUNDATIONS_CONTRIBUTION: return "contribution";
        default: return "unknown";
    }
}
