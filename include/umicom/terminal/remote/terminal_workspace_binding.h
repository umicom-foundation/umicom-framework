/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_workspace_binding.h
 *
 * PURPOSE:
 *   Define validated terminal workspace binding relationships between Framework-owned resources.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_WORKSPACE_BINDING_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_WORKSPACE_BINDING_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalWorkspaceBinding { char left_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; char right_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint64_t revision; bool enabled; } UmiTerminalRemoteTerminalWorkspaceBinding;
void umi_terminal_remote_terminal_workspace_binding_init(UmiTerminalRemoteTerminalWorkspaceBinding *value,const char *left_id,const char *right_id);
bool umi_terminal_remote_terminal_workspace_binding_valid(const UmiTerminalRemoteTerminalWorkspaceBinding *value);
uint64_t umi_terminal_remote_terminal_workspace_binding_fingerprint(const UmiTerminalRemoteTerminalWorkspaceBinding *value);
#ifdef __cplusplus
}
#endif
#endif
