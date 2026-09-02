/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/palette_model.c
 *
 * PURPOSE:
 *   Implement searchable component/tool palette metadata for visual designers, media tools and application builders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/palette_model.h"

/*
 * Initialise ws palette model from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_palette_model_init(UmiWsPaletteModel *model) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model != NULL) *model = (UmiWsPaletteModel){0};
}

/* Add ws palette model only after its inputs and available capacity have been checked. */
UmiStatus umi_ws_palette_model_add(UmiWsPaletteModel *model,
                                   const char *item_id,
                                   const char *label,
                                   const char *category) {
    UmiWsPaletteItem *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || !umi_ws_id_valid(item_id) || label == NULL || category == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_WS_MAX_PALETTE_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    item = &model->items[model->count++];
    *item = (UmiWsPaletteItem){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(item->item_id, sizeof(item->item_id), item_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(item->label, sizeof(item->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(item->category, sizeof(item->category), category) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_ws_copy_text(item->icon_name, sizeof(item->icon_name), "applications-graphics-symbolic");
}

/*
 * Provide the ws palette model count matching operation used by this module and its client
 * applications.
 */
size_t umi_ws_palette_model_count_matching(const UmiWsPaletteModel *model, const char *query) {
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || query == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < model->count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (query[0] == '\0' || strstr(model->items[index].label, query) != NULL || strstr(model->items[index].category, query) != NULL) ++count;
    }
    return count;
}
