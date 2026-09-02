/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_endpoint.c
 *
 * PURPOSE:
 *   Implement bounded remote endpoint identity and validation.
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
#include "umicom/terminal/remote/remote_endpoint.h"
#include <string.h>
/*
 * Initialise terminal remote remote endpoint from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_endpoint_init(UmiTerminalRemoteRemoteEndpoint *value,const char *host,uint16_t port,bool secure) { /* Apply this branch only when its contract condition is satisfied. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Apply this branch only when its contract condition is satisfied. */ if(host) (void)umi_terminal_remote_copy_text(value->host,sizeof(value->host),host); value->port=port; value->secure=secure; }
/*
 * Check that terminal remote remote endpoint satisfies its contract before another service
 * relies on it.
 */
bool umi_terminal_remote_remote_endpoint_valid(const UmiTerminalRemoteRemoteEndpoint *value) { return value&&value->host[0]!='\0'&&value->port!=0U; }
