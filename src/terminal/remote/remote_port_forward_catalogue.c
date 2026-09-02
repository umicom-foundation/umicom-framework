/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_port_forward_catalogue.c
 *
 * PURPOSE:
 *   Implement bounded remote port forward catalogue catalogue operations.
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
#include "umicom/terminal/remote/remote_port_forward_catalogue.h"
#include <string.h>
/*
 * Initialise terminal remote remote port forward catalogue from caller-provided values so
 * later operations receive a known state.
 */
void umi_terminal_remote_remote_port_forward_catalogue_init(UmiTerminalRemoteRemotePortForwardCatalogue *value) { /* Apply this branch only when its contract condition is satisfied. */ if (value) { (void)memset(value,0,sizeof(*value)); value->revision=1U; } }
/*
 * Find terminal remote remote port forward catalogue while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_remote_port_forward_catalogue_find(const UmiTerminalRemoteRemotePortForwardCatalogue *value,const char *id) { size_t i; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!value||!id) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<value->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(value->items[i].id,id)==0) return &value->items[i]; return NULL; }
/*
 * Add terminal remote remote port forward catalogue only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_terminal_remote_remote_port_forward_catalogue_add(UmiTerminalRemoteRemotePortForwardCatalogue *value,const char *id,const char *label) { /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!value||!id||id[0]=='\0'||!label) return UMI_STATUS_INVALID_ARGUMENT; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(umi_terminal_remote_remote_port_forward_catalogue_find(value,id)) return UMI_STATUS_ALREADY_EXISTS; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value->count>=UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; umi_terminal_remote_named_entry_init(&value->items[value->count],id,label); value->count++; value->revision++; return UMI_STATUS_OK; }
/*
 * Return the number of records represented by terminal remote remote port forward
 * catalogue enabled without changing their state.
 */
size_t umi_terminal_remote_remote_port_forward_catalogue_enabled_count(const UmiTerminalRemoteRemotePortForwardCatalogue *value) { size_t i,n=0U; /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!value) return 0U; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<value->count;++i) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(value->items[i].enabled) ++n; return n; }
