/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/status_strip.c
 *
 * PURPOSE:
 *   Implement reusable left/right status-strip items for mode, health, context, latency, branch and document state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/status_strip.h"

void umi_ws_status_strip_init(UmiWsStatusStrip *strip) {
    if (strip != NULL) *strip = (UmiWsStatusStrip){0};
}

UmiStatus umi_ws_status_strip_set(UmiWsStatusStrip *strip,
                                  const char *item_id,
                                  const char *text,
                                  UmiUiSeverity severity,
                                  bool align_right) {
    size_t index;
    UmiWsStatusItem *item = NULL;
    if (strip == NULL || !umi_ws_id_valid(item_id) || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < strip->count; ++index) {
        if (strcmp(strip->items[index].item_id, item_id) == 0) { item = &strip->items[index]; break; }
    }
    if (item == NULL) {
        if (strip->count >= UMI_WS_MAX_STATUS_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
        item = &strip->items[strip->count++];
        *item = (UmiWsStatusItem){0};
        if (umi_ws_copy_text(item->item_id, sizeof(item->item_id), item_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (umi_ws_copy_text(item->text, sizeof(item->text), text) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    item->severity = severity;
    item->align_right = align_right;
    return UMI_STATUS_OK;
}

const UmiWsStatusItem *umi_ws_status_strip_find(const UmiWsStatusStrip *strip, const char *item_id) {
    size_t index;
    if (strip == NULL || !umi_ws_id_valid(item_id)) return NULL;
    for (index = 0U; index < strip->count; ++index) if (strcmp(strip->items[index].item_id, item_id) == 0) return &strip->items[index];
    return NULL;
}
