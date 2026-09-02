/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/command_binding.c
 *
 * PURPOSE:
 *   Bind browser command events to stable Framework command IDs and explicit allow policies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/command_binding.h"

#include <string.h>
/*
 * Initialise native web command binding from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_native_web_command_binding_init(UmiNativeWebCommandBinding *binding, const char *element_id, const char *command_id, bool enabled){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(binding==NULL||element_id==NULL||command_id==NULL||command_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->element_id,sizeof(binding->element_id),element_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->command_id,sizeof(binding->command_id),command_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;binding->enabled=enabled;return UMI_STATUS_OK;}

