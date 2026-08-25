/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/process_lifecycle.h
 *
 * PURPOSE:
 *   Define process lifecycle transition policy for process lifecycle.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_LIFECYCLE_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_LIFECYCLE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
bool umi_terminal_remote_process_lifecycle_transition_allowed(UmiTerminalRemoteState from,UmiTerminalRemoteState to);
#ifdef __cplusplus
}
#endif
#endif
