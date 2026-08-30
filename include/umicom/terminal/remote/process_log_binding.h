/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/process_log_binding.h
 *
 * PURPOSE:
 *   Define validated process log binding relationships between Framework-owned resources.
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
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_LOG_BINDING_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_LOG_BINDING_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteProcessLogBinding { char left_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; char right_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint64_t revision; bool enabled; } UmiTerminalRemoteProcessLogBinding;
void umi_terminal_remote_process_log_binding_init(UmiTerminalRemoteProcessLogBinding *value,const char *left_id,const char *right_id);
bool umi_terminal_remote_process_log_binding_valid(const UmiTerminalRemoteProcessLogBinding *value);
uint64_t umi_terminal_remote_process_log_binding_fingerprint(const UmiTerminalRemoteProcessLogBinding *value);
#ifdef __cplusplus
}
#endif
#endif
