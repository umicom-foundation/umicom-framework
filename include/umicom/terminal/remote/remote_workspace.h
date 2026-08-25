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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_WORKSPACE_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_WORKSPACE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteWorkspace { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteRemoteWorkspace;
void umi_terminal_remote_remote_workspace_init(UmiTerminalRemoteRemoteWorkspace *value,const char *id,uint64_t limit);
UmiStatus umi_terminal_remote_remote_workspace_set_metric(UmiTerminalRemoteRemoteWorkspace *value,uint64_t metric);
bool umi_terminal_remote_remote_workspace_within_limit(const UmiTerminalRemoteRemoteWorkspace *value);
uint64_t umi_terminal_remote_remote_workspace_score(const UmiTerminalRemoteRemoteWorkspace *value);
#ifdef __cplusplus
}
#endif
#endif
