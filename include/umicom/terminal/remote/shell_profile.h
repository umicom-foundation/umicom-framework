/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/shell_profile.h
 *
 * PURPOSE:
 *   Model Framework-owned state for shell profile with stable identity and deterministic score.
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
#ifndef UMICOM_TERMINAL_REMOTE_SHELL_PROFILE_H
#define UMICOM_TERMINAL_REMOTE_SHELL_PROFILE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the terminal remote shell profile data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteShellProfile { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteShellProfile;
/**
 * Initialise terminal remote shell profile from caller-provided values so later operations
 * receive a known state.
 */
void umi_terminal_remote_shell_profile_init(UmiTerminalRemoteShellProfile *value,const char *id,uint64_t limit);
/**
 * Provide the terminal remote shell profile set metric operation used by this module and
 * its client applications.
 */
UmiStatus umi_terminal_remote_shell_profile_set_metric(UmiTerminalRemoteShellProfile *value,uint64_t metric);
/**
 * Provide the terminal remote shell profile within limit operation used by this module and
 * its client applications.
 */
bool umi_terminal_remote_shell_profile_within_limit(const UmiTerminalRemoteShellProfile *value);
/**
 * Provide the terminal remote shell profile score operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_remote_shell_profile_score(const UmiTerminalRemoteShellProfile *value);
#ifdef __cplusplus
}
#endif
#endif
