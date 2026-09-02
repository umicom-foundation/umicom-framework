/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_persistence.h
 *
 * PURPOSE:
 *   Track dirty revisions and save checkpoints for terminal persistence.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_PERSISTENCE_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_PERSISTENCE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote terminal persistence data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteTerminalPersistence { uint64_t revision; uint64_t saved_revision; } UmiTerminalRemoteTerminalPersistence;
/**
 * Initialise terminal remote terminal persistence from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_persistence_init(UmiTerminalRemoteTerminalPersistence *value);
/**
 * Provide the terminal remote terminal persistence touch operation used by this module and
 * its client applications.
 */
void umi_terminal_remote_terminal_persistence_touch(UmiTerminalRemoteTerminalPersistence *value);
/**
 * Provide the terminal remote terminal persistence dirty operation used by this module and
 * its client applications.
 */
bool umi_terminal_remote_terminal_persistence_dirty(const UmiTerminalRemoteTerminalPersistence *value);
/**
 * Provide the terminal remote terminal persistence mark saved operation used by this
 * module and its client applications.
 */
void umi_terminal_remote_terminal_persistence_mark_saved(UmiTerminalRemoteTerminalPersistence *value);
#ifdef __cplusplus
}
#endif
#endif
