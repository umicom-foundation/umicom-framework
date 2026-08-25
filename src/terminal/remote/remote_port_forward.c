/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_port_forward.c
 *
 * PURPOSE:
 *   Implement port-forward validation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_port_forward.h"
#include <string.h>
void umi_terminal_remote_remote_port_forward_init(UmiTerminalRemoteRemotePortForward *value,uint16_t local_port,const char *remote_host,uint16_t remote_port) { if(!value) return; (void)memset(value,0,sizeof(*value)); value->local_port=local_port; value->remote_port=remote_port; if(remote_host) (void)umi_terminal_remote_copy_text(value->remote_host,sizeof(value->remote_host),remote_host); value->enabled=true; }
bool umi_terminal_remote_remote_port_forward_valid(const UmiTerminalRemoteRemotePortForward *value) { return value&&value->enabled&&value->local_port!=0U&&value->remote_port!=0U&&value->remote_host[0]!='\0'; }
