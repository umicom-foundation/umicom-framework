/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/process_identity.h
 *
 * PURPOSE:
 *   Model Framework-owned state for process identity with stable identity and deterministic score.
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
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_IDENTITY_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_IDENTITY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote process identity data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteProcessIdentity { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteProcessIdentity;
/**
 * Initialise terminal remote process identity from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_process_identity_init(UmiTerminalRemoteProcessIdentity *value,const char *id,uint64_t limit);
/**
 * Provide the terminal remote process identity set metric operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_process_identity_set_metric(UmiTerminalRemoteProcessIdentity *value,uint64_t metric);
/**
 * Provide the terminal remote process identity within limit operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_process_identity_within_limit(const UmiTerminalRemoteProcessIdentity *value);
/**
 * Provide the terminal remote process identity score operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_remote_process_identity_score(const UmiTerminalRemoteProcessIdentity *value);
#ifdef __cplusplus
}
#endif
#endif
