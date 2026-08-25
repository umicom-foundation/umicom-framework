/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_debug_bridge.h
 *
 * PURPOSE:
 *   Define validated remote debug bridge relationships between Framework-owned resources.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_DEBUG_BRIDGE_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_DEBUG_BRIDGE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteDebugBridge { char left_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; char right_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint64_t revision; bool enabled; } UmiTerminalRemoteRemoteDebugBridge;
void umi_terminal_remote_remote_debug_bridge_init(UmiTerminalRemoteRemoteDebugBridge *value,const char *left_id,const char *right_id);
bool umi_terminal_remote_remote_debug_bridge_valid(const UmiTerminalRemoteRemoteDebugBridge *value);
uint64_t umi_terminal_remote_remote_debug_bridge_fingerprint(const UmiTerminalRemoteRemoteDebugBridge *value);
#ifdef __cplusplus
}
#endif
#endif
