/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/clipboard_payload.h
 *
 * PURPOSE:
 *   Represent semantic copied components without serialising toolkit objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_CLIPBOARD_PAYLOAD_H
#define UMICOM_DESIGNER_RAD_CLIPBOARD_PAYLOAD_H
#include "umicom/designer/rad/component_instance.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadClipboardPayload { UmiRadComponentInstance items[UMI_RAD_MAX_ITEMS]; size_t count; } UmiRadClipboardPayload;
UmiStatus umi_rad_clipboard_payload_init(UmiRadClipboardPayload *payload);
UmiStatus umi_rad_clipboard_payload_add(UmiRadClipboardPayload *payload,const UmiRadComponentInstance *item);
#ifdef __cplusplus
}
#endif
#endif
