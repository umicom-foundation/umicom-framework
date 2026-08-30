/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_capability.h
 *
 * PURPOSE:
 *   Model required/available capability flags for remote capability.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_CAPABILITY_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_CAPABILITY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef uint64_t UmiTerminalRemoteRemoteCapabilityFlags;
bool umi_terminal_remote_remote_capability_satisfies(UmiTerminalRemoteRemoteCapabilityFlags available,UmiTerminalRemoteRemoteCapabilityFlags required);
size_t umi_terminal_remote_remote_capability_count(UmiTerminalRemoteRemoteCapabilityFlags flags);
#ifdef __cplusplus
}
#endif
#endif
