/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/terminal_task_binding.h
 *
 * PURPOSE:
 *   Define validated terminal task binding relationships between Framework-owned resources.
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
#ifndef UMICOM_TERMINAL_REMOTE_TERMINAL_TASK_BINDING_H
#define UMICOM_TERMINAL_REMOTE_TERMINAL_TASK_BINDING_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteTerminalTaskBinding { char left_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; char right_id[UMI_TERMINAL_REMOTE_ID_CAPACITY]; uint64_t revision; bool enabled; } UmiTerminalRemoteTerminalTaskBinding;
void umi_terminal_remote_terminal_task_binding_init(UmiTerminalRemoteTerminalTaskBinding *value,const char *left_id,const char *right_id);
bool umi_terminal_remote_terminal_task_binding_valid(const UmiTerminalRemoteTerminalTaskBinding *value);
uint64_t umi_terminal_remote_terminal_task_binding_fingerprint(const UmiTerminalRemoteTerminalTaskBinding *value);
#ifdef __cplusplus
}
#endif
#endif
