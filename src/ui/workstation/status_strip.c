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

/*
 * Initialise ws status strip from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_status_strip_init(UmiWsStatusStrip *strip) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip != NULL) *strip = (UmiWsStatusStrip){0};
}

/*
 * Copy ws status strip into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_ws_status_strip_set(UmiWsStatusStrip *strip,
                                  const char *item_id,
                                  const char *text,
                                  UmiUiSeverity severity,
                                  bool align_right) {
    size_t index;
    UmiWsStatusItem *item = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip == NULL || !umi_ws_id_valid(item_id) || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < strip->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(strip->items[index].item_id, item_id) == 0) { item = &strip->items[index]; break; }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strip->count >= UMI_WS_MAX_STATUS_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
        item = &strip->items[strip->count++];
        *item = (UmiWsStatusItem){0};
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_ws_copy_text(item->item_id, sizeof(item->item_id), item_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(item->text, sizeof(item->text), text) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    item->severity = severity;
    item->align_right = align_right;
    return UMI_STATUS_OK;
}

/*
 * Find ws status strip while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsStatusItem *umi_ws_status_strip_find(const UmiWsStatusStrip *strip, const char *item_id) {
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strip == NULL || !umi_ws_id_valid(item_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < strip->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(strip->items[index].item_id, item_id) == 0) return &strip->items[index];
    return NULL;
}
