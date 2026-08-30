/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_capability.h
 *
 * PURPOSE:
 *   Model required/available capability flags for terminal capability.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_CAPABILITY_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_CAPABILITY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef uint64_t UmiTerminalRemoteTerminalCapabilityFlags;
bool umi_terminal_remote_terminal_capability_satisfies(UmiTerminalRemoteTerminalCapabilityFlags available,UmiTerminalRemoteTerminalCapabilityFlags required);
size_t umi_terminal_remote_terminal_capability_count(UmiTerminalRemoteTerminalCapabilityFlags flags);
#ifdef __cplusplus
}
#endif
#endif
