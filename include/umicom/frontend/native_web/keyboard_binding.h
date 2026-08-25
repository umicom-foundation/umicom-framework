/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/keyboard_binding.h
 *
 * PURPOSE:
 *   Normalize browser keyboard chords for the existing Framework keybinding/command layer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_KEYBOARD_BINDING_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_KEYBOARD_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"
typedef struct UmiNativeWebKeyboardBinding { char chord[96]; char command_id[UMI_NATIVE_WEB_ID_CAPACITY]; bool prevent_default; } UmiNativeWebKeyboardBinding;
/* Initialise validated keyboard binding state. */
UmiStatus umi_native_web_keyboard_binding_init(UmiNativeWebKeyboardBinding *binding, const char *chord, const char *command_id, bool prevent_default);

#ifdef __cplusplus
}
#endif
#endif
