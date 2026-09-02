/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_workspace.h
 *
 * PURPOSE:
 *   Model Framework-owned state for remote workspace with stable identity and deterministic score.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_WORKSPACE_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_WORKSPACE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote workspace data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteRemoteWorkspace { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteRemoteWorkspace;
/**
 * Initialise terminal remote remote workspace from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_workspace_init(UmiTerminalRemoteRemoteWorkspace *value,const char *id,uint64_t limit);
/**
 * Provide the terminal remote remote workspace set metric operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_remote_workspace_set_metric(UmiTerminalRemoteRemoteWorkspace *value,uint64_t metric);
/**
 * Provide the terminal remote remote workspace within limit operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_remote_workspace_within_limit(const UmiTerminalRemoteRemoteWorkspace *value);
/**
 * Provide the terminal remote remote workspace score operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_remote_remote_workspace_score(const UmiTerminalRemoteRemoteWorkspace *value);
#ifdef __cplusplus
}
#endif
#endif
