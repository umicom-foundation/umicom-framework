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
void umi_terminal_remote_remote_port_forward_catalogue_init(UmiTerminalRemoteRemotePortForwardCatalogue *value) { if (value) { (void)memset(value,0,sizeof(*value)); value->revision=1U; } }
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_remote_port_forward_catalogue_find(const UmiTerminalRemoteRemotePortForwardCatalogue *value,const char *id) { size_t i; if (!value||!id) return NULL; for(i=0U;i<value->count;++i) if(strcmp(value->items[i].id,id)==0) return &value->items[i]; return NULL; }
UmiStatus umi_terminal_remote_remote_port_forward_catalogue_add(UmiTerminalRemoteRemotePortForwardCatalogue *value,const char *id,const char *label) { if(!value||!id||id[0]=='\0'||!label) return UMI_STATUS_INVALID_ARGUMENT; if(umi_terminal_remote_remote_port_forward_catalogue_find(value,id)) return UMI_STATUS_ALREADY_EXISTS; if(value->count>=UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; umi_terminal_remote_named_entry_init(&value->items[value->count],id,label); value->count++; value->revision++; return UMI_STATUS_OK; }
size_t umi_terminal_remote_remote_port_forward_catalogue_enabled_count(const UmiTerminalRemoteRemotePortForwardCatalogue *value) { size_t i,n=0U; if(!value) return 0U; for(i=0U;i<value->count;++i) if(value->items[i].enabled) ++n; return n; }
