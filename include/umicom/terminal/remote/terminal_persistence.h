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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_PERSISTENCE_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_PERSISTENCE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalPersistence { uint64_t revision; uint64_t saved_revision; } UmiTerminalRemoteTerminalPersistence;
void umi_terminal_remote_terminal_persistence_init(UmiTerminalRemoteTerminalPersistence *value);
void umi_terminal_remote_terminal_persistence_touch(UmiTerminalRemoteTerminalPersistence *value);
bool umi_terminal_remote_terminal_persistence_dirty(const UmiTerminalRemoteTerminalPersistence *value);
void umi_terminal_remote_terminal_persistence_mark_saved(UmiTerminalRemoteTerminalPersistence *value);
#ifdef __cplusplus
}
#endif
#endif
