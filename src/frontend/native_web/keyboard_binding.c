/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/keyboard_binding.c
 *
 * PURPOSE:
 *   Normalize browser keyboard chords for the existing Framework keybinding/command layer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/keyboard_binding.h"

#include <string.h>
UmiStatus umi_native_web_keyboard_binding_init(UmiNativeWebKeyboardBinding *binding, const char *chord, const char *command_id, bool prevent_default){UmiStatus s;if(binding==NULL||chord==NULL||command_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(binding,0,sizeof(*binding));s=umi_native_web_copy_text(binding->chord,sizeof(binding->chord),chord);if(s!=UMI_STATUS_OK)return s;s=umi_native_web_copy_text(binding->command_id,sizeof(binding->command_id),command_id);if(s!=UMI_STATUS_OK)return s;binding->prevent_default=prevent_default;return UMI_STATUS_OK;}

