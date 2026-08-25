/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_workspace_mount.c
 *
 * PURPOSE:
 *   Implement remote workspace mount policy state.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_workspace_mount.h"
#include <string.h>
void umi_terminal_remote_remote_workspace_mount_init(UmiTerminalRemoteRemoteWorkspaceMount *value,const char *root,bool read_only) { if(!value) return; (void)memset(value,0,sizeof(*value)); if(root) (void)umi_terminal_remote_copy_text(value->root,sizeof(value->root),root); value->read_only=read_only; }
bool umi_terminal_remote_remote_workspace_mount_allows_write(const UmiTerminalRemoteRemoteWorkspaceMount *value) { return value&&value->root[0]!='\0'&&!value->read_only; }
