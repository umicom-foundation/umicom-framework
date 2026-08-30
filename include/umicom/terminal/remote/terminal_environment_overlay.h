/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_environment_overlay.h
 *
 * PURPOSE:
 *   Model Framework-owned state for terminal environment overlay with stable identity and deterministic score.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_ENVIRONMENT_OVERLAY_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_ENVIRONMENT_OVERLAY_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalEnvironmentOverlay { UmiTerminalRemoteNamedEntry identity; uint64_t metric; uint64_t limit; } UmiTerminalRemoteTerminalEnvironmentOverlay;
void umi_terminal_remote_terminal_environment_overlay_init(UmiTerminalRemoteTerminalEnvironmentOverlay *value,const char *id,uint64_t limit);
UmiStatus umi_terminal_remote_terminal_environment_overlay_set_metric(UmiTerminalRemoteTerminalEnvironmentOverlay *value,uint64_t metric);
bool umi_terminal_remote_terminal_environment_overlay_within_limit(const UmiTerminalRemoteTerminalEnvironmentOverlay *value);
uint64_t umi_terminal_remote_terminal_environment_overlay_score(const UmiTerminalRemoteTerminalEnvironmentOverlay *value);
#ifdef __cplusplus
}
#endif
#endif
