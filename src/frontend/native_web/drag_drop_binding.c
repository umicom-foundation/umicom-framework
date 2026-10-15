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
#include "../../base/value_construction_internal.h"

#include <string.h>
/*
 * Initialise native web drag drop binding from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_native_web_drag_drop_binding_init(UmiNativeWebDragDropBinding *binding, const char *source_id, const char *target_id, const char *payload_type, bool allowed){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(binding==NULL||source_id==NULL||target_id==NULL||payload_type==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->source_id,sizeof(binding->source_id),source_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->target_id,sizeof(binding->target_id),target_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->payload_type,sizeof(binding->payload_type),payload_type);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;binding->allowed=allowed;return UMI_STATUS_OK;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_native_web_drag_drop_binding_init_build_value(UmiNativeWebDragDropBinding *binding, const char *source_id, const char *target_id, const char *payload_type, bool allowed){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(binding==NULL||source_id==NULL||target_id==NULL||payload_type==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->source_id,sizeof(binding->source_id),source_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->target_id,sizeof(binding->target_id),target_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->payload_type,sizeof(binding->payload_type),payload_type);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;binding->allowed=allowed;return UMI_STATUS_OK;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_native_web_drag_drop_binding_init, UmiNativeWebDragDropBinding, binding,
    (UmiNativeWebDragDropBinding *binding, const char *source_id, const char *target_id, const char *payload_type, bool allowed),
    umi_native_web_drag_drop_binding_init_build_value(candidate, source_id, target_id, payload_type, allowed))
