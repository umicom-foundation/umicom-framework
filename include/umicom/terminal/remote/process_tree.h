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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_PROCESS_TREE_H
#define UMICOM_TERMINAL_REMOTE_PROCESS_TREE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_TERMINAL_REMOTE_PROCESS_TREE_CAPACITY 32U
/**
 * Represent the terminal remote process tree node data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteProcessTreeNode { uint64_t pid; uint64_t parent_pid; } UmiTerminalRemoteProcessTreeNode;
/**
 * Represent the terminal remote process tree data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalRemoteProcessTree { UmiTerminalRemoteProcessTreeNode items[UMI_TERMINAL_REMOTE_PROCESS_TREE_CAPACITY]; size_t count; } UmiTerminalRemoteProcessTree;
/**
 * Initialise terminal remote process tree from caller-provided values so later operations
 * receive a known state.
 */
void umi_terminal_remote_process_tree_init(UmiTerminalRemoteProcessTree *value);
/**
 * Add terminal remote process tree only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_terminal_remote_process_tree_add(UmiTerminalRemoteProcessTree *value,uint64_t pid,uint64_t parent_pid);
/**
 * Provide the terminal remote process tree is descendant operation used by this module and
 * its client applications.
 */
bool umi_terminal_remote_process_tree_is_descendant(const UmiTerminalRemoteProcessTree *value,uint64_t pid,uint64_t ancestor_pid);
#ifdef __cplusplus
}
#endif
#endif
