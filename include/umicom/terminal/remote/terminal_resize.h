/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_resize.h
 *
 * PURPOSE:
 *   Validate terminal rows and columns against safe bounds.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_RESIZE_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_RESIZE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote terminal resize data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteTerminalResize { uint32_t columns; uint32_t rows; uint64_t revision; } UmiTerminalRemoteTerminalResize;
/**
 * Initialise terminal remote terminal resize from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_resize_init(UmiTerminalRemoteTerminalResize *value,uint32_t columns,uint32_t rows);
/**
 * Copy terminal remote terminal resize into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_terminal_remote_terminal_resize_set(UmiTerminalRemoteTerminalResize *value,uint32_t columns,uint32_t rows);
#ifdef __cplusplus
}
#endif
#endif
