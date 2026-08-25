/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_workspace.c
 *
 * PURPOSE:
 *   Implement bounded state and scoring for remote workspace.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_workspace.h"
void umi_terminal_remote_remote_workspace_init(UmiTerminalRemoteRemoteWorkspace *value,const char *id,uint64_t limit) { if(value) { umi_terminal_remote_named_entry_init(&value->identity,id,"remote workspace"); value->metric=0U; value->limit=limit; } }
UmiStatus umi_terminal_remote_remote_workspace_set_metric(UmiTerminalRemoteRemoteWorkspace *value,uint64_t metric) { if(!value||!umi_terminal_remote_named_entry_valid(&value->identity)) return UMI_STATUS_INVALID_ARGUMENT; value->metric=metric; value->identity.revision++; return UMI_STATUS_OK; }
bool umi_terminal_remote_remote_workspace_within_limit(const UmiTerminalRemoteRemoteWorkspace *value) { return value&&umi_terminal_remote_named_entry_valid(&value->identity)&&(value->limit==0U||value->metric<=value->limit); }
uint64_t umi_terminal_remote_remote_workspace_score(const UmiTerminalRemoteRemoteWorkspace *value) { if(!umi_terminal_remote_remote_workspace_within_limit(value)) return 0U; return umi_terminal_remote_fingerprint_text(value->identity.id) ^ value->metric ^ (value->limit<<1U); }
