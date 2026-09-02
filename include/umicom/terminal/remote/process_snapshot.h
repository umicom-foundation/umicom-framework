/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/process_snapshot.h
 *
 * PURPOSE:
 *   Model Framework-owned state for process snapshot with stable identity and deterministic score.
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
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_SNAPSHOT_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_SNAPSHOT_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote process snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteProcessSnapshot { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteProcessSnapshot;
/**
 * Initialise terminal remote process snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_process_snapshot_init(UmiTerminalRemoteProcessSnapshot *value,const char *id,uint64_t limit);
/**
 * Provide the terminal remote process snapshot set metric operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_process_snapshot_set_metric(UmiTerminalRemoteProcessSnapshot *value,uint64_t metric);
/**
 * Provide the terminal remote process snapshot within limit operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_process_snapshot_within_limit(const UmiTerminalRemoteProcessSnapshot *value);
/**
 * Provide the terminal remote process snapshot score operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_remote_process_snapshot_score(const UmiTerminalRemoteProcessSnapshot *value);
#ifdef __cplusplus
}
#endif
#endif
