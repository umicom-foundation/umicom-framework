/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/clipboard_payload.h
 *
 * PURPOSE:
 *   Represent semantic copied components without serialising toolkit objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_CLIPBOARD_PAYLOAD_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_CLIPBOARD_PAYLOAD_H
#include "umicom/designer/visual_designer/component_instance.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer clipboard payload data shared with callers of this public contract.
 */
typedef struct UmiRadClipboardPayload { UmiRadComponentInstance items[UMI_RAD_MAX_ITEMS]; size_t count; } UmiRadClipboardPayload;
/**
 * Initialise visual designer clipboard payload from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_clipboard_payload_init(UmiRadClipboardPayload *payload);
/**
 * Add visual designer clipboard payload only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_rad_clipboard_payload_add(UmiRadClipboardPayload *payload,const UmiRadComponentInstance *item);
#ifdef __cplusplus
}
#endif
#endif
