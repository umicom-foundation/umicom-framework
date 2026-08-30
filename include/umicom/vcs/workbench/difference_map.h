/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/difference_map.h
 *
 * PURPOSE:
 *   Build a compact, renderer-neutral difference map from canonical compare
 *   hunks for overview rulers and Beyond Compare-style navigation strips.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_DIFFERENCE_MAP_H
#define UMICOM_VCS_WORKBENCH_DIFFERENCE_MAP_H

#include "umicom/vcs/advanced/types.h"
#include "umicom/vcs/workbench/compare_model.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsWorkbenchDifferenceBand {
    uint32_t struct_size;
    uint32_t api_version;
    size_t hunk_index;
    size_t first_row;
    size_t last_row;
    uint32_t start_permyriad;
    uint32_t end_permyriad;
    UmiVcsDiffKind dominant_kind;
    size_t change_count;
    int selected;
} UmiVcsWorkbenchDifferenceBand;

typedef struct UmiVcsWorkbenchDifferenceMap {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsWorkbenchDifferenceBand bands[
        UMI_VCS_WORKBENCH_MAX_DIFFERENCE_BANDS];
    size_t count;
    size_t row_count;
    size_t selected_index;
    int truncated;
    uint64_t revision;
} UmiVcsWorkbenchDifferenceMap;

void umi_vcs_workbench_difference_map_init(
    UmiVcsWorkbenchDifferenceMap *map);
UmiStatus umi_vcs_workbench_difference_map_build(
    UmiVcsWorkbenchDifferenceMap *map,
    const UmiVcsWorkbenchCompareModel *model);
UmiStatus umi_vcs_workbench_difference_map_select(
    UmiVcsWorkbenchDifferenceMap *map,
    size_t index);
const UmiVcsWorkbenchDifferenceBand *
umi_vcs_workbench_difference_map_band_at(
    const UmiVcsWorkbenchDifferenceMap *map,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_DIFFERENCE_MAP_H */
