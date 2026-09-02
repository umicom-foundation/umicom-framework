/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/clipboard_payload.c
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
#include "umicom/designer/visual_designer/clipboard_payload.h"
#include <string.h>
/*
 * Initialise visual designer clipboard payload from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_clipboard_payload_init(UmiRadClipboardPayload *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(p,0,sizeof *p);return UMI_STATUS_OK;}
/*
 * Add visual designer clipboard payload only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_rad_clipboard_payload_add(UmiRadClipboardPayload *p,const UmiRadComponentInstance *i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||i==NULL||!umi_rad_component_instance_is_valid(i))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_RAD_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;p->items[p->count++]=*i;return UMI_STATUS_OK;}
