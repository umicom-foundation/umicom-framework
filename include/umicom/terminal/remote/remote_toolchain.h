/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_toolchain.h
 *
 * PURPOSE:
 *   Define validated remote toolchain relationships between Framework-owned resources.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_TOOLCHAIN_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_TOOLCHAIN_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteToolchain { char left_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; char right_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint64_t revision; bool enabled; } UmiTerminalRemoteRemoteToolchain;
void umi_terminal_remote_remote_toolchain_init(UmiTerminalRemoteRemoteToolchain *value,const char *left_id,const char *right_id);
bool umi_terminal_remote_remote_toolchain_valid(const UmiTerminalRemoteRemoteToolchain *value);
uint64_t umi_terminal_remote_remote_toolchain_fingerprint(const UmiTerminalRemoteRemoteToolchain *value);
#ifdef __cplusplus
}
#endif
#endif
