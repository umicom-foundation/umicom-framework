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

#include <string.h>
UmiStatus umi_native_web_context_binding_init(UmiNativeWebContextBinding *binding, const char *element_id, const char *channel_id, const char *context_type, bool bidirectional){UmiStatus s;if(binding==NULL||element_id==NULL||channel_id==NULL||context_type==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->element_id,sizeof(binding->element_id),element_id);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->channel_id,sizeof(binding->channel_id),channel_id);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->context_type,sizeof(binding->context_type),context_type);if(s!=UMI_STATUS_OK)return s;binding->bidirectional=bidirectional;return UMI_STATUS_OK;}

