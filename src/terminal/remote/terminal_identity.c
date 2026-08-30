/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_identity.c
 *
 * PURPOSE:
 *   Implement bounded state and scoring for terminal identity.
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
#include "umicom/terminal/remote/terminal_identity.h"
void umi_terminal_remote_terminal_identity_init(UmiTerminalRemoteTerminalIdentity *value,const char *id,uint64_t limit) { if(value) { umi_terminal_remote_named_entry_init(&value->identity,id,"terminal identity"); value->metric=0U; value->limit=limit; } }
UmiStatus umi_terminal_remote_terminal_identity_set_metric(UmiTerminalRemoteTerminalIdentity *value,uint64_t metric) { if(!value||!umi_terminal_remote_named_entry_valid(&value->identity)) return UMI_STATUS_INVALID_ARGUMENT; value->metric=metric; value->identity.revision++; return UMI_STATUS_OK; }
bool umi_terminal_remote_terminal_identity_within_limit(const UmiTerminalRemoteTerminalIdentity *value) { return value&&umi_terminal_remote_named_entry_valid(&value->identity)&&(value->limit==0U||value->metric<=value->limit); }
uint64_t umi_terminal_remote_terminal_identity_score(const UmiTerminalRemoteTerminalIdentity *value) { if(!umi_terminal_remote_terminal_identity_within_limit(value)) return 0U; return umi_terminal_remote_fingerprint_text(value->identity.id) ^ value->metric ^ (value->limit<<1U); }
