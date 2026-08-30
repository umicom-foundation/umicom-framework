/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/process_tree.c
 *
 * PURPOSE:
 *   Implement bounded process tree insertion and ancestry traversal.
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
#include "umicom/terminal/remote/process_tree.h"
#include <string.h>
void umi_terminal_remote_process_tree_init(UmiTerminalRemoteProcessTree *value) { if(value) (void)memset(value,0,sizeof(*value)); }
UmiStatus umi_terminal_remote_process_tree_add(UmiTerminalRemoteProcessTree *value,uint64_t pid,uint64_t parent_pid) { size_t i; if(!value||pid==0U||pid==parent_pid) return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<value->count;++i) if(value->items[i].pid==pid) return UMI_STATUS_ALREADY_EXISTS; if(value->count>=UMI_TERMINAL_REMOTE_PROCESS_TREE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; value->items[value->count].pid=pid; value->items[value->count].parent_pid=parent_pid; value->count++; return UMI_STATUS_OK; }
bool umi_terminal_remote_process_tree_is_descendant(const UmiTerminalRemoteProcessTree *value,uint64_t pid,uint64_t ancestor_pid) { size_t steps=0U; if(!value||pid==0U||ancestor_pid==0U) return false; while(pid!=0U&&steps<=value->count) { size_t i; if(pid==ancestor_pid) return true; for(i=0U;i<value->count;++i) if(value->items[i].pid==pid) { pid=value->items[i].parent_pid; break; } if(i==value->count) return false; steps++; } return false; }
