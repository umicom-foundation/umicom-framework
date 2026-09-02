/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_identity.h
 *
 * PURPOSE:
 *   Model Framework-owned state for terminal identity with stable identity and deterministic score.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_IDENTITY_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_IDENTITY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote terminal identity data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteTerminalIdentity { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteTerminalIdentity;
/**
 * Initialise terminal remote terminal identity from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_identity_init(UmiTerminalRemoteTerminalIdentity *value,const char *id,uint64_t limit);
/**
 * Provide the terminal remote terminal identity set metric operation used by this module
 * and its client applications.
 */
UmiStatus umi_terminal_remote_terminal_identity_set_metric(UmiTerminalRemoteTerminalIdentity *value,uint64_t metric);
/**
 * Provide the terminal remote terminal identity within limit operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_terminal_identity_within_limit(const UmiTerminalRemoteTerminalIdentity *value);
/**
 * Provide the terminal remote terminal identity score operation used by this module and
 * its client applications.
 */
uint64_t umi_terminal_remote_terminal_identity_score(const UmiTerminalRemoteTerminalIdentity *value);
#ifdef __cplusplus
}
#endif
#endif
