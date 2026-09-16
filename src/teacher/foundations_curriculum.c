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
    }
};

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
