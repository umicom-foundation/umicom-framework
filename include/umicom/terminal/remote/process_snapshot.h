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
typedef struct UmiTerminalRemoteProcessSnapshot { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteProcessSnapshot;
void umi_terminal_remote_process_snapshot_init(UmiTerminalRemoteProcessSnapshot *value,const char *id,uint64_t limit);
UmiStatus umi_terminal_remote_process_snapshot_set_metric(UmiTerminalRemoteProcessSnapshot *value,uint64_t metric);
bool umi_terminal_remote_process_snapshot_within_limit(const UmiTerminalRemoteProcessSnapshot *value);
uint64_t umi_terminal_remote_process_snapshot_score(const UmiTerminalRemoteProcessSnapshot *value);
#ifdef __cplusplus
}
#endif
#endif
