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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_ENVIRONMENT_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_ENVIRONMENT_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote remote environment data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteRemoteEnvironment { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteRemoteEnvironment;
/**
 * Initialise terminal remote remote environment from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_environment_init(UmiTerminalRemoteRemoteEnvironment *value,const char *id,uint64_t limit);
/**
 * Provide the terminal remote remote environment set metric operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_remote_environment_set_metric(UmiTerminalRemoteRemoteEnvironment *value,uint64_t metric);
/**
 * Provide the terminal remote remote environment within limit operation used by this
 * module and its client applications.
 */
bool umi_terminal_remote_remote_environment_within_limit(const UmiTerminalRemoteRemoteEnvironment *value);
/**
 * Provide the terminal remote remote environment score operation used by this module and
 * its client applications.
 */
uint64_t umi_terminal_remote_remote_environment_score(const UmiTerminalRemoteRemoteEnvironment *value);
#ifdef __cplusplus
}
#endif
#endif
