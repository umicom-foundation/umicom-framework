/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/keyboard_binding.h
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
/**
 * Represent the native web keyboard binding data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeWebKeyboardBinding { char chord[96]; char command_id[UMI_NATIVE_WEB_ID_CAPACITY]; bool prevent_default; } UmiNativeWebKeyboardBinding;
/* Initialise validated keyboard binding state. */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_native_web_keyboard_binding_init(UmiNativeWebKeyboardBinding *binding, const char *chord, const char *command_id, bool prevent_default);

#ifdef __cplusplus
}
#endif
#endif
