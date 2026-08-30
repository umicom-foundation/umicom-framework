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
typedef struct UmiTerminalRemoteTerminalIdentity { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteTerminalIdentity;
void umi_terminal_remote_terminal_identity_init(UmiTerminalRemoteTerminalIdentity *value,const char *id,uint64_t limit);
UmiStatus umi_terminal_remote_terminal_identity_set_metric(UmiTerminalRemoteTerminalIdentity *value,uint64_t metric);
bool umi_terminal_remote_terminal_identity_within_limit(const UmiTerminalRemoteTerminalIdentity *value);
uint64_t umi_terminal_remote_terminal_identity_score(const UmiTerminalRemoteTerminalIdentity *value);
#ifdef __cplusplus
}
#endif
#endif
