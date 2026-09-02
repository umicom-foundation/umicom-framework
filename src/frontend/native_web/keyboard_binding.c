/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/keyboard_binding.c
 *
 * PURPOSE:
 *   Normalize browser keyboard chords for the existing Framework keybinding/command layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/keyboard_binding.h"

#include <string.h>
/*
 * Initialise native web keyboard binding from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_native_web_keyboard_binding_init(UmiNativeWebKeyboardBinding *binding, const char *chord, const char *command_id, bool prevent_default){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(binding==NULL||chord==NULL||command_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->chord,sizeof(binding->chord),chord);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->command_id,sizeof(binding->command_id),command_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;binding->prevent_default=prevent_default;return UMI_STATUS_OK;}

