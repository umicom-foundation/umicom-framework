/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_difference_map.c
 *
 * PURPOSE:
 *   Verify normalized difference bands follow canonical compare hunks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/vcs/workbench/difference_map.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The original automatic fixture can exceed the native Windows stack before its first assertion. The replacement owns the same records on the heap; retain the earlier test and its comments for review. */
#if 0
int main(void)
{
    UmiVcsWorkbenchCompareModel model;
    UmiVcsWorkbenchDifferenceMap map;
    UmiVcsAdvancedCompareSide left;
    UmiVcsAdvancedCompareSide right;
    const UmiVcsWorkbenchDifferenceBand *band;
    umi_vcs_advanced_compare_side_init(&left);
    umi_vcs_advanced_compare_side_init(&right);
    assert(umi_vcs_advanced_compare_side_set(
               &left, "a.c", "HEAD", "Left", 1) == UMI_STATUS_OK);
    assert(umi_vcs_advanced_compare_side_set(
               &right, "a.c", "", "Right", 0) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_compare_model_open(
               &model, "map-1", &left, &right,
               "a\nb\nc\n", "a\nx\nc\n", NULL) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_difference_map_build(&map, &model) ==
           UMI_STATUS_OK);
    assert(map.count == model.hunk_count);
    band = umi_vcs_workbench_difference_map_band_at(&map, 0U);
    assert(band != NULL);
    assert(band->end_permyriad <= 10000U);
    assert(umi_vcs_workbench_difference_map_select(&map, 0U) ==
           UMI_STATUS_OK);
    assert(map.bands[0].selected);
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
    UmiVcsWorkbenchDifferenceMap map;
} FixtureStorage;

static int CheckFixture(FixtureStorage *state)
{
    UmiVcsAdvancedCompareSide left;
    UmiVcsAdvancedCompareSide right;
    const UmiVcsWorkbenchDifferenceBand *band;
    umi_vcs_advanced_compare_side_init(&left);
    umi_vcs_advanced_compare_side_init(&right);
    assert(umi_vcs_advanced_compare_side_set(
               &left, "a.c", "HEAD", "Left", 1) == UMI_STATUS_OK);
    assert(umi_vcs_advanced_compare_side_set(
               &right, "a.c", "", "Right", 0) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_compare_model_open(
               &state->model, "map-1", &left, &right,
               "a\nb\nc\n", "a\nx\nc\n", NULL) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_difference_map_build(&state->map, &state->model) ==
           UMI_STATUS_OK);
    assert(state->map.count == state->model.hunk_count);
    band = umi_vcs_workbench_difference_map_band_at(&state->map, 0U);
    assert(band != NULL);
    assert(band->end_permyriad <= 10000U);
    assert(umi_vcs_workbench_difference_map_select(&state->map, 0U) ==
           UMI_STATUS_OK);
    assert(state->map.bands[0].selected);
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
