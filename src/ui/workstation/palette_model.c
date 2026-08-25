/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/palette_model.c
 *
 * PURPOSE:
 *   Implement searchable component/tool palette metadata for visual designers, media tools and application builders.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/palette_model.h"

void umi_ws_palette_model_init(UmiWsPaletteModel *model) {
    if (model != NULL) *model = (UmiWsPaletteModel){0};
}

UmiStatus umi_ws_palette_model_add(UmiWsPaletteModel *model,
                                   const char *item_id,
                                   const char *label,
                                   const char *category) {
    UmiWsPaletteItem *item;
    if (model == NULL || !umi_ws_id_valid(item_id) || label == NULL || category == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (model->count >= UMI_WS_MAX_PALETTE_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    item = &model->items[model->count++];
    *item = (UmiWsPaletteItem){0};
    if (umi_ws_copy_text(item->item_id, sizeof(item->item_id), item_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(item->label, sizeof(item->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(item->category, sizeof(item->category), category) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_ws_copy_text(item->icon_name, sizeof(item->icon_name), "applications-graphics-symbolic");
}

size_t umi_ws_palette_model_count_matching(const UmiWsPaletteModel *model, const char *query) {
    size_t index;
    size_t count = 0U;
    if (model == NULL || query == NULL) return 0U;
    for (index = 0U; index < model->count; ++index) {
        if (query[0] == '\0' || strstr(model->items[index].label, query) != NULL || strstr(model->items[index].category, query) != NULL) ++count;
    }
    return count;
}
