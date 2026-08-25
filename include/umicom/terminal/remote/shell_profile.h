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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_SHELL_PROFILE_H
#define UMICOM_TERMINAL_REMOTE_SHELL_PROFILE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteShellProfile { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteShellProfile;
void umi_terminal_remote_shell_profile_init(UmiTerminalRemoteShellProfile *value,const char *id,uint64_t limit);
UmiStatus umi_terminal_remote_shell_profile_set_metric(UmiTerminalRemoteShellProfile *value,uint64_t metric);
bool umi_terminal_remote_shell_profile_within_limit(const UmiTerminalRemoteShellProfile *value);
uint64_t umi_terminal_remote_shell_profile_score(const UmiTerminalRemoteShellProfile *value);
#ifdef __cplusplus
}
#endif
#endif
