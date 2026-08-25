/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_environment.h
 *
 * PURPOSE:
 *   Model Framework-owned state for remote environment with stable identity and deterministic score.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_ENVIRONMENT_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_ENVIRONMENT_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteEnvironment { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteRemoteEnvironment;
void umi_terminal_remote_remote_environment_init(UmiTerminalRemoteRemoteEnvironment *value,const char *id,uint64_t limit);
UmiStatus umi_terminal_remote_remote_environment_set_metric(UmiTerminalRemoteRemoteEnvironment *value,uint64_t metric);
bool umi_terminal_remote_remote_environment_within_limit(const UmiTerminalRemoteRemoteEnvironment *value);
uint64_t umi_terminal_remote_remote_environment_score(const UmiTerminalRemoteRemoteEnvironment *value);
#ifdef __cplusplus
}
#endif
#endif
