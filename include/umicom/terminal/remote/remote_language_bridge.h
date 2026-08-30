/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_language_bridge.h
 *
 * PURPOSE:
 *   Define validated remote language bridge relationships between Framework-owned resources.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_LANGUAGE_BRIDGE_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_LANGUAGE_BRIDGE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteLanguageBridge { char left_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; char right_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint64_t revision; bool enabled; } UmiTerminalRemoteRemoteLanguageBridge;
void umi_terminal_remote_remote_language_bridge_init(UmiTerminalRemoteRemoteLanguageBridge *value,const char *left_id,const char *right_id);
bool umi_terminal_remote_remote_language_bridge_valid(const UmiTerminalRemoteRemoteLanguageBridge *value);
uint64_t umi_terminal_remote_remote_language_bridge_fingerprint(const UmiTerminalRemoteRemoteLanguageBridge *value);
#ifdef __cplusplus
}
#endif
#endif
