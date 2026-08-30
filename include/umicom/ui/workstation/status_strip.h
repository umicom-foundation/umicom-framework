/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/status_strip.h
 *
 * PURPOSE:
 *   Define reusable left/right status-strip items for mode, health, context, latency, branch and document state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_STATUS_STRIP_H
#define UMICOM_UI_WORKSTATION_STATUS_STRIP_H

#include <stdbool.h>
#include <stddef.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsStatusItem {
    char item_id[UMI_UI_ID_CAPACITY];
    char text[UMI_UI_TEXT_CAPACITY];
    UmiUiSeverity severity;
    bool align_right;
} UmiWsStatusItem;

typedef struct UmiWsStatusStrip {
    UmiWsStatusItem items[UMI_WS_MAX_STATUS_ITEMS];
    size_t count;
} UmiWsStatusStrip;

void umi_ws_status_strip_init(UmiWsStatusStrip *strip);
UmiStatus umi_ws_status_strip_set(UmiWsStatusStrip *strip,
                                  const char *item_id,
                                  const char *text,
                                  UmiUiSeverity severity,
                                  bool align_right);
const UmiWsStatusItem *umi_ws_status_strip_find(const UmiWsStatusStrip *strip, const char *item_id);

#ifdef __cplusplus
}
#endif

#endif
