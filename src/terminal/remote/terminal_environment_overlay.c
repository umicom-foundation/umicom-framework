/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_environment_overlay.c
 *
 * PURPOSE:
 *   Implement bounded state and scoring for terminal environment overlay.
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
#include "umicom/terminal/remote/terminal_environment_overlay.h"
/*
 * Initialise terminal remote terminal environment overlay from caller-provided values so
 * later operations receive a known state.
 */
void umi_terminal_remote_terminal_environment_overlay_init(UmiTerminalRemoteTerminalEnvironmentOverlay *value,const char *id,uint64_t limit) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value) { umi_terminal_remote_named_entry_init(&value->identity,id,"terminal environment overlay"); value->metric=0U; value->limit=limit; } }
/*
 * Provide the terminal remote terminal environment overlay set metric operation used by
 * this module and its client applications.
 */
UmiStatus umi_terminal_remote_terminal_environment_overlay_set_metric(UmiTerminalRemoteTerminalEnvironmentOverlay *value,uint64_t metric) { /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value||!umi_terminal_remote_named_entry_valid(&value->identity)) return UMI_STATUS_INVALID_ARGUMENT; value->metric=metric; value->identity.revision++; return UMI_STATUS_OK; }
/*
 * Provide the terminal remote terminal environment overlay within limit operation used by
 * this module and its client applications.
 */
bool umi_terminal_remote_terminal_environment_overlay_within_limit(const UmiTerminalRemoteTerminalEnvironmentOverlay *value) { return value&&umi_terminal_remote_named_entry_valid(&value->identity)&&(value->limit==0U||value->metric<=value->limit); }
/*
 * Provide the terminal remote terminal environment overlay score operation used by this
 * module and its client applications.
 */
uint64_t umi_terminal_remote_terminal_environment_overlay_score(const UmiTerminalRemoteTerminalEnvironmentOverlay *value) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!umi_terminal_remote_terminal_environment_overlay_within_limit(value)) return 0U; return umi_terminal_remote_fingerprint_text(value->identity.id) ^ value->metric ^ (value->limit<<1U); }
