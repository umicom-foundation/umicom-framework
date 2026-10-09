/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_side_by_side.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience side by side.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/diff_builder.h"
#include "umicom/ai_developer_experience/side_by_side.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The original automatic fixture can exceed the native Windows stack before its first assertion. The replacement owns the same records on the heap; retain the earlier test and its comments for review. */
#if 0
int main(void)
{
    UmiAiDeveloperFileDiff diff;
    UmiAiDeveloperSideBySideDiff split;

    assert(umi_ai_developer_diff_build(
        "a.c", UMI_AI_CODING_PATCH_MODIFY,
        "a\nold\nc\n", "a\nnew\nc\n",
        1U, 2U, &diff) == UMI_STATUS_OK);
    assert(umi_ai_developer_side_by_side_build(
        &diff, &split) == UMI_STATUS_OK);
    assert(split.row_count == 3U);
    assert(split.rows[1].kind == UMI_AI_DEVELOPER_SIDE_CHANGED);
    return 0;
}


#endif
#include <stdlib.h>
#include <stdio.h>

/* Large bounded records belong to this explicit fixture owner, not the native
 * thread stack. Each process still creates a fresh independent model and runs
 * the original semantic assertions; allocation failure is a test failure. */
typedef struct FixtureStorage {
    UmiAiDeveloperFileDiff diff;
    UmiAiDeveloperSideBySideDiff split;
} FixtureStorage;

static int CheckFixture(FixtureStorage *state)
{

    assert(umi_ai_developer_diff_build(
        "a.c", UMI_AI_CODING_PATCH_MODIFY,
        "a\nold\nc\n", "a\nnew\nc\n",
        1U, 2U, &state->diff) == UMI_STATUS_OK);
    assert(umi_ai_developer_side_by_side_build(
        &state->diff, &state->split) == UMI_STATUS_OK);
    assert(state->split.row_count == 3U);
    assert(state->split.rows[1].kind == UMI_AI_DEVELOPER_SIDE_CHANGED);
    return 0;
}

int main(void)
{
    FixtureStorage *state = calloc(1U, sizeof *state);
    if (state == NULL) {
        fputs("Cannot allocate fixture storage\n", stderr);
        return 1;
    }
    int result = CheckFixture(state);
    free(state);
    return result;
}
