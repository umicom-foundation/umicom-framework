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
/*
 * Initialise terminal remote process tree from caller-provided values so later operations
 * receive a known state.
 */
void umi_terminal_remote_process_tree_init(UmiTerminalRemoteProcessTree *value) { /* Apply this branch only when its contract condition is satisfied. */ if(value) (void)memset(value,0,sizeof(*value)); }
/*
 * Add terminal remote process tree only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_terminal_remote_process_tree_add(UmiTerminalRemoteProcessTree *value,uint64_t pid,uint64_t parent_pid) { size_t i; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!value||pid==0U||pid==parent_pid) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<value->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value->items[i].pid==pid) return UMI_STATUS_ALREADY_EXISTS; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value->count>=UMI_TERMINAL_REMOTE_PROCESS_TREE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; value->items[value->count].pid=pid; value->items[value->count].parent_pid=parent_pid; value->count++; return UMI_STATUS_OK; }
/*
 * Provide the terminal remote process tree is descendant operation used by this module and
 * its client applications.
 */
bool umi_terminal_remote_process_tree_is_descendant(const UmiTerminalRemoteProcessTree *value,uint64_t pid,uint64_t ancestor_pid) { size_t steps=0U; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!value||pid==0U||ancestor_pid==0U) return false; /* Continue only while work remains available; the loop body advances the state on each pass. */ while(pid!=0U&&steps<=value->count) { size_t i; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(pid==ancestor_pid) return true; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<value->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value->items[i].pid==pid) { pid=value->items[i].parent_pid; break; } /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(i==value->count) return false; steps++; } return false; }
