/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_workspace_mount.h
 *
 * PURPOSE:
 *   Model remote workspace mount roots and read-only enforcement.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_WORKSPACE_MOUNT_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_WORKSPACE_MOUNT_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote workspace mount data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteRemoteWorkspaceMount { char root[UMI_TERMINAL_REMOTE_PATH_CAPACITY]; bool read_only; } UmiTerminalRemoteRemoteWorkspaceMount;
/**
 * Initialise terminal remote remote workspace mount from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_workspace_mount_init(UmiTerminalRemoteRemoteWorkspaceMount *value,const char *root,bool read_only);
/**
 * Write terminal remote remote workspace mount allows in its stable representation and
 * report capacity or input failures to the caller.
 */
bool umi_terminal_remote_remote_workspace_mount_allows_write(const UmiTerminalRemoteRemoteWorkspaceMount *value);
#ifdef __cplusplus
}
#endif
#endif
