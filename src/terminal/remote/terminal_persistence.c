/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_persistence.c
 *
 * PURPOSE:
 *   Implement revision checkpointing for terminal persistence.
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
#include "umicom/terminal/remote/terminal_persistence.h"
void umi_terminal_remote_terminal_persistence_init(UmiTerminalRemoteTerminalPersistence *value) { if(value) { value->revision=1U; value->saved_revision=1U; } }
void umi_terminal_remote_terminal_persistence_touch(UmiTerminalRemoteTerminalPersistence *value) { if(value) value->revision++; }
bool umi_terminal_remote_terminal_persistence_dirty(const UmiTerminalRemoteTerminalPersistence *value) { return value&&value->revision!=value->saved_revision; }
void umi_terminal_remote_terminal_persistence_mark_saved(UmiTerminalRemoteTerminalPersistence *value) { if(value) value->saved_revision=value->revision; }
