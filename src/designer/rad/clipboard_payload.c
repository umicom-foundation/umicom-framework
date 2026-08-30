/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/clipboard_payload.c
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
#include "umicom/designer/rad/clipboard_payload.h"
#include <string.h>
UmiStatus umi_rad_clipboard_payload_init(UmiRadClipboardPayload *p){if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof *p);return UMI_STATUS_OK;}
UmiStatus umi_rad_clipboard_payload_add(UmiRadClipboardPayload *p,const UmiRadComponentInstance *i){if(p==NULL||i==NULL||!umi_rad_component_instance_is_valid(i))return UMI_STATUS_INVALID_ARGUMENT;if(p->count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;p->items[p->count++]=*i;return UMI_STATUS_OK;}
