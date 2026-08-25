/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/process_tree.h
 *
 * PURPOSE:
 *   Model bounded process parentage and descendant queries for supervised terminal tasks.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_TREE_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_TREE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TERMINAL_REMOTE_PROCESS_TREE_CAPACITY 32U
typedef struct UmiTerminalRemoteProcessTreeNode { uint64_t pid; uint64_t parent_pid; } UmiTerminalRemoteProcessTreeNode;
typedef struct UmiTerminalRemoteProcessTree { UmiTerminalRemoteProcessTreeNode items[UMI_TERMINAL_REMOTE_PROCESS_TREE_CAPACITY]; size_t count; } UmiTerminalRemoteProcessTree;
void umi_terminal_remote_process_tree_init(UmiTerminalRemoteProcessTree *value);
UmiStatus umi_terminal_remote_process_tree_add(UmiTerminalRemoteProcessTree *value,uint64_t pid,uint64_t parent_pid);
bool umi_terminal_remote_process_tree_is_descendant(const UmiTerminalRemoteProcessTree *value,uint64_t pid,uint64_t ancestor_pid);
#ifdef __cplusplus
}
#endif
#endif
