/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/layout_template.c
 *
 * PURPOSE:
 *   Implement reusable named layout templates composed from semantic workstation surfaces and dock regions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/layout_template.h"

UmiStatus umi_ws_layout_template_init(UmiWsLayoutTemplate *layout,
                                      const char *layout_id,
                                      const char *label,
                                      UmiWsApplicationDomain domain) {
    if (layout == NULL || !umi_ws_id_valid(layout_id) || label == NULL || label[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    *layout = (UmiWsLayoutTemplate){0};
    if (umi_ws_copy_text(layout->layout_id, sizeof(layout->layout_id), layout_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_ws_copy_text(layout->label, sizeof(layout->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    layout->domain = domain;
    layout->variant = UMI_WS_LAYOUT_DESKTOP;
    layout->locked_by_default = true;
    return UMI_STATUS_OK;
}

UmiStatus umi_ws_layout_template_add(UmiWsLayoutTemplate *layout,
                                     const char *surface_id,
                                     UmiWsDockRegion region,
                                     int32_t row,
                                     int32_t column,
                                     double weight) {
    UmiWsLayoutItem *item;
    size_t index;
    if (layout == NULL || !umi_ws_id_valid(surface_id)) return UMI_STATUS_INVALID_ARGUMENT;
    if (region < UMI_WS_DOCK_LEFT || region > UMI_WS_DOCK_FLOATING || row < 0 || column < 0) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < layout->count; ++index) {
        if (strcmp(layout->items[index].surface_id, surface_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    }
    if (layout->count >= UMI_WS_MAX_LAYOUT_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    item = &layout->items[layout->count];
    *item = (UmiWsLayoutItem){0};
    if (umi_ws_copy_text(item->surface_id, sizeof(item->surface_id), surface_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    item->region = region;
    item->row = row;
    item->column = column;
    item->row_span = 1;
    item->column_span = 1;
    item->weight = (weight <= 0.0) ? 1.0 : weight;
    item->visible = true;
    ++layout->count;
    return UMI_STATUS_OK;
}

const UmiWsLayoutItem *umi_ws_layout_template_find(const UmiWsLayoutTemplate *layout,
                                                   const char *surface_id) {
    size_t index;
    if (layout == NULL || !umi_ws_id_valid(surface_id)) return NULL;
    for (index = 0U; index < layout->count; ++index) {
        if (strcmp(layout->items[index].surface_id, surface_id) == 0) return &layout->items[index];
    }
    return NULL;
}

uint64_t umi_ws_layout_template_fingerprint(const UmiWsLayoutTemplate *layout) {
    size_t index;
    uint64_t hash;
    if (layout == NULL) return 0U;
    hash = umi_ws_hash_text(layout->layout_id);
    for (index = 0U; index < layout->count; ++index) {
        hash ^= umi_ws_hash_text(layout->items[index].surface_id);
        hash ^= ((uint64_t)layout->items[index].region << ((index % 7U) + 1U));
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}
