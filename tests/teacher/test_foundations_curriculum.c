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
    if (umi_teacher_foundations_curriculum_count() != 16U) return EXIT_FAILURE;
    first = umi_teacher_foundations_curriculum_at(0U);
    last = umi_teacher_foundations_curriculum_find(
        "foundations.pull-request");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (first == NULL || last == NULL || first->sequence != 1U ||
        last->sequence != 16U) return EXIT_FAILURE;
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
        umi_teacher_learning_plan_count(&plan) != 16U ||
        strcmp(umi_teacher_learning_plan_current(&plan), first->id) != 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
