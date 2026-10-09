/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_compare_model.c
 *
 * PURPOSE:
 *   Verify canonical diff data is composed into one navigable compare model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/vcs/workbench/compare_model.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The original automatic fixture can exceed the native Windows stack before its first assertion. The replacement owns the same records on the heap; retain the earlier test and its comments for review. */
#if 0
int main(void)
{
    UmiVcsWorkbenchCompareModel model;
    UmiVcsAdvancedCompareSide left;
    UmiVcsAdvancedCompareSide right;
    umi_vcs_advanced_compare_side_init(&left);
    umi_vcs_advanced_compare_side_init(&right);
    assert(umi_vcs_advanced_compare_side_set(
               &left, "src/a.c", "HEAD", "Working", 1) == UMI_STATUS_OK);
    assert(umi_vcs_advanced_compare_side_set(
               &right, "src/a.c", "", "Index", 0) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_compare_model_open(
               &model, "compare-1", &left, &right,
               "one\ntwo\nthree\n", "one\nchanged\nthree\nfour\n", NULL) ==
           UMI_STATUS_OK);
    assert(model.ready);
    assert(model.row_count > 0U);
    assert(model.hunk_count > 0U);
    assert(umi_vcs_workbench_compare_model_row_at(&model, 0U) != NULL);
    assert(umi_vcs_workbench_compare_model_hunk_at(&model, 0U) != NULL);
    assert(umi_vcs_workbench_compare_model_set_view_mode(
               &model, UMI_VCS_WORKBENCH_INLINE) == UMI_STATUS_OK);
    assert(model.view_mode == UMI_VCS_WORKBENCH_INLINE);
    return 0;
}

#endif
#include <stdlib.h>
#include <stdio.h>

/* Large bounded records belong to this explicit fixture owner, not the native
 * thread stack. Each process still creates a fresh independent model and runs
 * the original semantic assertions; allocation failure is a test failure. */
typedef struct FixtureStorage {
    UmiVcsWorkbenchCompareModel model;
} FixtureStorage;

static int CheckFixture(FixtureStorage *state)
{
    UmiVcsAdvancedCompareSide left;
    UmiVcsAdvancedCompareSide right;
    umi_vcs_advanced_compare_side_init(&left);
    umi_vcs_advanced_compare_side_init(&right);
    assert(umi_vcs_advanced_compare_side_set(
               &left, "src/a.c", "HEAD", "Working", 1) == UMI_STATUS_OK);
    assert(umi_vcs_advanced_compare_side_set(
               &right, "src/a.c", "", "Index", 0) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_compare_model_open(
               &state->model, "compare-1", &left, &right,
               "one\ntwo\nthree\n", "one\nchanged\nthree\nfour\n", NULL) ==
           UMI_STATUS_OK);
    assert(state->model.ready);
    assert(state->model.row_count > 0U);
    assert(state->model.hunk_count > 0U);
    assert(umi_vcs_workbench_compare_model_row_at(&state->model, 0U) != NULL);
    assert(umi_vcs_workbench_compare_model_hunk_at(&state->model, 0U) != NULL);
    assert(umi_vcs_workbench_compare_model_set_view_mode(
               &state->model, UMI_VCS_WORKBENCH_INLINE) == UMI_STATUS_OK);
    assert(state->model.view_mode == UMI_VCS_WORKBENCH_INLINE);
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
