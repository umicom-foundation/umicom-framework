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
