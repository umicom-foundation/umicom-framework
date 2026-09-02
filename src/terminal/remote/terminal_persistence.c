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
/*
 * Initialise terminal remote terminal persistence from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_persistence_init(UmiTerminalRemoteTerminalPersistence *value) { /* Apply this branch only when its contract condition is satisfied. */ if(value) { value->revision=1U; value->saved_revision=1U; } }
/*
 * Provide the terminal remote terminal persistence touch operation used by this module and
 * its client applications.
 */
void umi_terminal_remote_terminal_persistence_touch(UmiTerminalRemoteTerminalPersistence *value) { /* Apply this branch only when its contract condition is satisfied. */ if(value) value->revision++; }
/*
 * Provide the terminal remote terminal persistence dirty operation used by this module and
 * its client applications.
 */
bool umi_terminal_remote_terminal_persistence_dirty(const UmiTerminalRemoteTerminalPersistence *value) { return value&&value->revision!=value->saved_revision; }
/*
 * Provide the terminal remote terminal persistence mark saved operation used by this
 * module and its client applications.
 */
void umi_terminal_remote_terminal_persistence_mark_saved(UmiTerminalRemoteTerminalPersistence *value) { /* Apply this branch only when its contract condition is satisfied. */ if(value) value->saved_revision=value->revision; }
