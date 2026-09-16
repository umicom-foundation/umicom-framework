/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/teacher/test_foundations_curriculum.c
 *
 * PURPOSE:
 *   Verify the beginner journey is ordered, findable and compatible with the
 *   existing Framework learning-plan contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "umicom/teacher/foundations_curriculum.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTeacherLearningPlan plan;
    const UmiTeacherFoundationsLesson *first;
    const UmiTeacherFoundationsLesson *last;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_teacher_foundations_curriculum_count() != 20U) return EXIT_FAILURE;
    first = umi_teacher_foundations_curriculum_at(0U);
    last = umi_teacher_foundations_curriculum_find(
        "foundations.assembly");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (first == NULL || last == NULL || first->sequence != 1U ||
        last->sequence != 20U) return EXIT_FAILURE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_teacher_foundations_curriculum_next(first->id) == NULL) {
        return EXIT_FAILURE;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_teacher_foundations_curriculum_next(last->id) != NULL) {
        return EXIT_FAILURE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_teacher_foundations_curriculum_minutes() < 800U) {
        return EXIT_FAILURE;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_teacher_foundations_curriculum_plan(&plan) != UMI_STATUS_OK ||
        umi_teacher_learning_plan_count(&plan) != 20U ||
        strcmp(umi_teacher_learning_plan_current(&plan), first->id) != 0) {
        return EXIT_FAILURE;
    }
    /* IDs, rather than positions or display titles, identify saved progress.
     * Every original ID stays findable after new lessons are inserted. */
    const char *const originalIds[] = {
        "foundations.welcome", "foundations.files", "foundations.terminal",
        "foundations.toolchain", "foundations.c-hello", "foundations.c-data",
        "foundations.c-functions", "foundations.c-memory", "foundations.c-modules",
        "foundations.git", "foundations.clone", "foundations.framework",
        "foundations.components", "foundations.change", "foundations.branch",
        "foundations.pull-request"
    };
    for (size_t index = 0U; index < sizeof(originalIds) / sizeof(originalIds[0]); ++index) {
        if (umi_teacher_foundations_curriculum_find(originalIds[index]) == NULL)
            return EXIT_FAILURE;
    }
    for (size_t index = 0U; index < umi_teacher_foundations_curriculum_count(); ++index) {
        const UmiTeacherFoundationsLesson *lesson = umi_teacher_foundations_curriculum_at(index);
        if (lesson == NULL || lesson->sequence != index + 1U ||
            strncmp(lesson->resource_path, "framework/docs/learning/", sizeof("framework/docs/learning/") - 1U) != 0 ||
            lesson->title[0] == '\0' || lesson->exercise[0] == '\0') return EXIT_FAILURE;
        if (strcmp(plan.ids[index], lesson->id) != 0) return EXIT_FAILURE;
        for (size_t other = 0U; other < index; ++other) {
            if (strcmp(lesson->id, umi_teacher_foundations_curriculum_at(other)->id) == 0)
                return EXIT_FAILURE;
        }
    }
    if (umi_teacher_foundations_curriculum_at(20U) != NULL ||
        umi_teacher_foundations_curriculum_find(NULL) != NULL ||
        umi_teacher_foundations_curriculum_find("missing.lesson") != NULL ||
        umi_teacher_foundations_curriculum_plan(NULL) != UMI_STATUS_INVALID_ARGUMENT)
        return EXIT_FAILURE;
    /* Display/resource naming can evolve without resetting persisted IDs. */
    const UmiTeacherFoundationsLesson *gitLesson =
        umi_teacher_foundations_curriculum_find("foundations.git");
    const UmiTeacherFoundationsLesson *mainLesson =
        umi_teacher_foundations_curriculum_find("foundations.branch");
    const UmiTeacherFoundationsLesson *publishLesson =
        umi_teacher_foundations_curriculum_find("foundations.pull-request");
    if (gitLesson == NULL || mainLesson == NULL || publishLesson == NULL ||
        strcmp(gitLesson->resource_path, "framework/docs/learning/git-first-commit.html") != 0 ||
        strcmp(mainLesson->title, "Work safely on main") != 0 ||
        strcmp(publishLesson->title, "Publish a change and update Linux") != 0)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
