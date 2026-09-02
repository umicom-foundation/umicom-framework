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

/**
 * Represent the ws status item data shared with callers of this public contract.
 */
typedef struct UmiWsStatusItem {
    char item_id[UMI_UI_ID_CAPACITY];
    char text[UMI_UI_TEXT_CAPACITY];
    UmiUiSeverity severity;
    bool align_right;
} UmiWsStatusItem;

/**
 * Represent the ws status strip data shared with callers of this public contract.
 */
typedef struct UmiWsStatusStrip {
    UmiWsStatusItem items[UMI_WS_MAX_STATUS_ITEMS];
    size_t count;
} UmiWsStatusStrip;

/**
 * Initialise ws status strip from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_status_strip_init(UmiWsStatusStrip *strip);
/**
 * Copy ws status strip into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_ws_status_strip_set(UmiWsStatusStrip *strip,
                                  const char *item_id,
                                  const char *text,
                                  UmiUiSeverity severity,
                                  bool align_right);
/**
 * Find ws status strip while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiWsStatusItem *umi_ws_status_strip_find(const UmiWsStatusStrip *strip, const char *item_id);

#ifdef __cplusplus
}
#endif

#endif
