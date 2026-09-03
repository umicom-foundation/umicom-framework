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

static const UmiTeacherFoundationsLesson FOUNDATIONS_LESSONS[] = {
    {
        "foundations.welcome", "Welcome to building software",
        "Learn what a program, source file, compiler, repository and application are.",
        "Match five everyday explanations to the correct software words.",
        "docs/learning/01-welcome.md", UMI_TEACHER_FOUNDATIONS_ORIENTATION,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_FOUNDATION,
        1U, 20U, 70U, 0, 0, 0
    },
    {
        "foundations.files", "Folders, files and safe paths",
        "Create a practice folder, recognise a file extension and use an absolute path.",
        "Create a folder named umicom-practice and explain where it lives.",
        "docs/learning/02-files-and-folders.md", UMI_TEACHER_FOUNDATIONS_ORIENTATION,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_FOUNDATION,
        2U, 25U, 70U, 0, 0, 0
    },
    {
        "foundations.terminal", "Your first terminal commands",
        "Open PowerShell or a Linux terminal, change folder and list its contents.",
        "Move into the practice folder, list it, then return to its parent.",
        "docs/learning/03-terminal.md", UMI_TEACHER_FOUNDATIONS_TOOLS,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_FOUNDATION,
        3U, 30U, 70U, 0, 0, 0
    },
    {
        "foundations.toolchain", "Check the development tools",
        "Use the Umicom bootstrap doctor and understand each OK or MISSING result.",
        "Run the doctor action and write down what Git, CMake, Ninja and GCC do.",
        "docs/learning/04-development-tools.md", UMI_TEACHER_FOUNDATIONS_TOOLS,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_FOUNDATION,
        4U, 35U, 75U, 0, 0, 0
    },
    {
        "foundations.c-hello", "A tiny C program",
        "Read main, include a header, print a message and return a success code.",
        "Create, compile and run a program that prints Hello from Umicom.",
        "framework/docs/learning/c-language-and-safety.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_FOUNDATION,
        5U, 45U, 75U, 1, 0, 0
    },
    {
        "foundations.c-data", "Names, values and decisions in C",
        "Use meaningful variables, basic data types, if statements and loops.",
        "Write a loop that labels scores as ready or needs more practice.",
        "framework/docs/learning/c-language-and-safety.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        6U, 60U, 75U, 1, 0, 0
    },
    {
        "foundations.c-functions", "Functions and clear contracts",
        "Split work into small functions with inputs, outputs and useful names.",
        "Extract score validation into a function and test three inputs.",
        "framework/docs/learning/c-language-and-safety.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        7U, 60U, 75U, 1, 0, 0
    },
    {
        "foundations.c-memory", "Pointers, arrays and memory safety",
        "Understand addresses, NULL, array bounds, ownership and cleanup.",
        "Find and repair three deliberately unsafe pointer examples.",
        "framework/docs/learning/c-language-and-safety.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        8U, 75U, 80U, 1, 0, 0
    },
    {
        "foundations.c-modules", "Headers, source files and tests",
        "Create a small public contract, keep implementation private and verify behavior.",
        "Build one header, one source file and one focused test without warnings.",
        "framework/docs/learning/c-language-and-safety.html", UMI_TEACHER_FOUNDATIONS_C_PROGRAMMING,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        9U, 75U, 80U, 1, 0, 0
    },
    {
        "foundations.git", "Git without mystery",
        "Learn working tree, staging area, commit, branch, remote and push.",
        "Make a practice change, inspect it, stage it and create a local commit.",
        "docs/learning/10-git-basics.md", UMI_TEACHER_FOUNDATIONS_TOOLS,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_BEGINNER,
        10U, 60U, 75U, 0, 1, 0
    },
    {
        "foundations.clone", "Clone Umicom and its submodules",
        "Download the parent repository and understand why applications are pinned submodules.",
        "Clone with the bootstrap script, then inspect the parent and Framework status.",
        "docs/learning/11-clone-umicom.md", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_BEGINNER,
        11U, 45U, 80U, 0, 1, 0
    },
    {
        "foundations.framework", "How Umicom Framework fits together",
        "Follow a reusable contract from public header through implementation, test and application use.",
        "Trace one Studio panel back to its Framework-owned component contract.",
        "docs/learning/12-framework-architecture.md", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        12U, 75U, 80U, 1, 1, 0
    },
    {
        "foundations.components", "Build applications like Lego",
        "Compose panels, commands, layouts and services instead of copying product logic.",
        "Sketch a small application from existing component recipe slots.",
        "docs/learning/13-components-and-layouts.md", UMI_TEACHER_FOUNDATIONS_FRAMEWORK,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        13U, 75U, 80U, 1, 1, 0
    },
    {
        "foundations.change", "Make one quality Framework change",
        "Write a stable contract, implementation, comments, focused test and beginner documentation.",
        "Implement a small additive component and pass its focused checks.",
        "docs/learning/14-first-framework-change.md", UMI_TEACHER_FOUNDATIONS_CONTRIBUTION,
        UMI_TEACHER_LANGUAGE_C23, UMI_TEACHER_LEVEL_BEGINNER,
        14U, 120U, 85U, 1, 1, 0
    },
    {
        "foundations.branch", "Prepare a contribution branch",
        "Create a short-lived branch, review the diff and write a meaningful commit.",
        "Create feature/learning-practice and prepare one clean commit.",
        "docs/learning/15-branches-and-commits.md", UMI_TEACHER_FOUNDATIONS_CONTRIBUTION,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_BEGINNER,
        15U, 45U, 80U, 0, 1, 1
    },
    {
        "foundations.pull-request", "Open and improve a pull request",
        "Fork, push, explain the change, request review and respond to feedback.",
        "Open a practice pull request with purpose, evidence and test results.",
        "docs/learning/16-pull-requests.md", UMI_TEACHER_FOUNDATIONS_CONTRIBUTION,
        UMI_TEACHER_LANGUAGE_GENERAL, UMI_TEACHER_LEVEL_BEGINNER,
        16U, 60U, 85U, 0, 1, 1
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
