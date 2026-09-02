/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/drag_drop_binding.c
 *
 * PURPOSE:
 *   Normalize browser drag/drop operations into semantic layout-edit intents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/drag_drop_binding.h"

#include <string.h>
/*
 * Initialise native web drag drop binding from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_native_web_drag_drop_binding_init(UmiNativeWebDragDropBinding *binding, const char *source_id, const char *target_id, const char *payload_type, bool allowed){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(binding==NULL||source_id==NULL||target_id==NULL||payload_type==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->source_id,sizeof(binding->source_id),source_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->target_id,sizeof(binding->target_id),target_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->payload_type,sizeof(binding->payload_type),payload_type);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;binding->allowed=allowed;return UMI_STATUS_OK;}

