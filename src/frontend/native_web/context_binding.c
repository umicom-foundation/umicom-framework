/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/context_binding.c
 *
 * PURPOSE:
 *   Bind browser selections to typed Framework context-channel identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/context_binding.h"
#include "../../base/value_construction_internal.h"

#include <string.h>
/*
 * Initialise native web context binding from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_native_web_context_binding_init(UmiNativeWebContextBinding *binding, const char *element_id, const char *channel_id, const char *context_type, bool bidirectional){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(binding==NULL||element_id==NULL||channel_id==NULL||context_type==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->element_id,sizeof(binding->element_id),element_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->channel_id,sizeof(binding->channel_id),channel_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->context_type,sizeof(binding->context_type),context_type);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;binding->bidirectional=bidirectional;return UMI_STATUS_OK;}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_native_web_context_binding_init_build_value(UmiNativeWebContextBinding *binding, const char *element_id, const char *channel_id, const char *context_type, bool bidirectional){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(binding==NULL||element_id==NULL||channel_id==NULL||context_type==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->element_id,sizeof(binding->element_id),element_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->channel_id,sizeof(binding->channel_id),channel_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->context_type,sizeof(binding->context_type),context_type);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;binding->bidirectional=bidirectional;return UMI_STATUS_OK;}

UMI_DEFINE_STAGED_VALUE_INIT(umi_native_web_context_binding_init, UmiNativeWebContextBinding, binding,
    (UmiNativeWebContextBinding *binding, const char *element_id, const char *channel_id, const char *context_type, bool bidirectional),
    umi_native_web_context_binding_init_build_value(candidate, element_id, channel_id, context_type, bidirectional))
