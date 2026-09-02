/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/clipboard_payload.h
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
#ifndef UMICOM_DESIGNER_RAD_CLIPBOARD_PAYLOAD_H
#define UMICOM_DESIGNER_RAD_CLIPBOARD_PAYLOAD_H
#include "umicom/designer/rad/component_instance.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad clipboard payload data shared with callers of this public contract.
 */
typedef struct UmiRadClipboardPayload { UmiRadComponentInstance items[UMI_RAD_MAX_ITEMS]; size_t count; } UmiRadClipboardPayload;
/**
 * Initialise rad clipboard payload from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_clipboard_payload_init(UmiRadClipboardPayload *payload);
/**
 * Add rad clipboard payload only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_rad_clipboard_payload_add(UmiRadClipboardPayload *payload,const UmiRadComponentInstance *item);
#ifdef __cplusplus
}
#endif
#endif
