/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/difference_map.c
 *
 * PURPOSE:
 *   Project compare hunks into normalized overview bands without toolkit code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/difference_map.h"

#include <string.h>

static UmiVcsDiffKind dominant_kind(
    const UmiVcsWorkbenchCompareModel *model,
    const UmiDeveloperDiffHunk *hunk)
{
    size_t inserted = 0U;
    size_t deleted = 0U;
    size_t replaced = 0U;
    size_t row;

    for (row = hunk->first_row;
         row <= hunk->last_row && row < model->row_count;
         ++row) {
        switch (model->rows[row].kind) {
            case UMI_DEVELOPER_DIFF_INSERT: inserted += 1U; break;
            case UMI_DEVELOPER_DIFF_DELETE: deleted += 1U; break;
            case UMI_DEVELOPER_DIFF_REPLACE: replaced += 1U; break;
            case UMI_DEVELOPER_DIFF_EQUAL:
            default: break;
        }
    }
    if (replaced >= inserted && replaced >= deleted) {
        return UMI_VCS_DIFF_MODIFIED;
    }
    return inserted >= deleted ? UMI_VCS_DIFF_ADDED : UMI_VCS_DIFF_DELETED;
}

void umi_vcs_workbench_difference_map_init(
    UmiVcsWorkbenchDifferenceMap *map)
{
    if (map == NULL) return;
    (void)memset(map, 0, sizeof(*map));
    map->struct_size = (uint32_t)sizeof(*map);
    map->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    map->revision = 1U;
}

UmiStatus umi_vcs_workbench_difference_map_build(
    UmiVcsWorkbenchDifferenceMap *map,
    const UmiVcsWorkbenchCompareModel *model)
{
    size_t index;

    if (map == NULL || model == NULL || !model->ready) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_vcs_workbench_difference_map_init(map);
    map->row_count = model->row_count;
    map->count = model->hunk_count;
    if (map->count > UMI_VCS_WORKBENCH_MAX_DIFFERENCE_BANDS) {
        map->count = UMI_VCS_WORKBENCH_MAX_DIFFERENCE_BANDS;
        map->truncated = 1;
    }
    map->selected_index = model->navigation.current_index;
    for (index = 0U; index < map->count; ++index) {
        const UmiDeveloperDiffHunk *hunk = &model->hunks[index];
        UmiVcsWorkbenchDifferenceBand *band = &map->bands[index];
        band->struct_size = (uint32_t)sizeof(*band);
        band->api_version = UMI_VCS_WORKBENCH_API_VERSION;
        band->hunk_index = index;
        band->first_row = hunk->first_row;
        band->last_row = hunk->last_row;
        band->change_count = hunk->changed_rows;
        band->dominant_kind = dominant_kind(model, hunk);
        band->start_permyriad = model->row_count > 0U
            ? (uint32_t)((hunk->first_row * 10000U) / model->row_count) : 0U;
        band->end_permyriad = model->row_count > 0U
            ? (uint32_t)(((hunk->last_row + 1U) * 10000U) /
                         model->row_count) : 0U;
        if (band->end_permyriad > 10000U) band->end_permyriad = 10000U;
        band->selected = index == map->selected_index;
    }
    map->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_difference_map_select(
    UmiVcsWorkbenchDifferenceMap *map,
    size_t index)
{
    size_t item;

    if (map == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= map->count) return UMI_STATUS_NOT_FOUND;
    map->selected_index = index;
    for (item = 0U; item < map->count; ++item) {
        map->bands[item].selected = item == index;
    }
    map->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiVcsWorkbenchDifferenceBand *
umi_vcs_workbench_difference_map_band_at(
    const UmiVcsWorkbenchDifferenceMap *map,
    size_t index)
{
    return map != NULL && index < map->count ? &map->bands[index] : NULL;
}
