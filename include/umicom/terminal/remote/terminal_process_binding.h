/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_process_binding.h
 *
 * PURPOSE:
 *   Define validated terminal process binding relationships between Framework-owned resources.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_PROCESS_BINDING_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_PROCESS_BINDING_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote terminal process binding data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteTerminalProcessBinding { char left_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; char right_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint64_t revision; bool enabled; } UmiTerminalRemoteTerminalProcessBinding;
/**
 * Initialise terminal remote terminal process binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_process_binding_init(UmiTerminalRemoteTerminalProcessBinding *value,const char *left_id,const char *right_id);
/**
 * Check that terminal remote terminal process binding satisfies its contract before
 * another service relies on it.
 */
bool umi_terminal_remote_terminal_process_binding_valid(const UmiTerminalRemoteTerminalProcessBinding *value);
/**
 * Provide the terminal remote terminal process binding fingerprint operation used by this
 * module and its client applications.
 */
uint64_t umi_terminal_remote_terminal_process_binding_fingerprint(const UmiTerminalRemoteTerminalProcessBinding *value);
#ifdef __cplusplus
}
#endif
#endif
