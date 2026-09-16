/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning/programming/records_enums/main.c
 *
 * PURPOSE:
 *   Structures, enumerations and state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum LessonState { NOT_STARTED, ACTIVE, COMPLETED } LessonState;
typedef struct Progress { LessonState state; unsigned score; } Progress;
static bool ValidProgress(const Progress *progress)
{
    if (progress==NULL || progress->score>100U) return false;
    switch (progress->state) {
    case NOT_STARTED: return progress->score==0U;
    case ACTIVE: return true;
    case COMPLETED: return progress->score>=75U;
    default: return false;
    }
}
int main(void)
{
    Progress progress={NOT_STARTED,0U};
    if (!ValidProgress(&progress)) return EXIT_FAILURE;
    progress=(Progress){COMPLETED,74U};
    if (ValidProgress(&progress)) return EXIT_FAILURE;
    progress=(Progress){COMPLETED,75U};
    if (!ValidProgress(&progress)) return EXIT_FAILURE;
    progress.state=(LessonState)99;
    if (ValidProgress(&progress)) return EXIT_FAILURE;
    puts("completion requires a declared state and score >= 75");
    puts("PASS: records_enums");
    return EXIT_SUCCESS;
}
