/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_endpoint.h
 *
 * PURPOSE:
 *   Represent a remote-development endpoint with host/port and transport identity.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_ENDPOINT_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_ENDPOINT_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemoteEndpoint { char host[UMI_TERMINAL_REMOTE_TEXT_CAPACITY]; uint16_t port; bool secure; } UmiTerminalRemoteRemoteEndpoint;
void umi_terminal_remote_remote_endpoint_init(UmiTerminalRemoteRemoteEndpoint *value,const char *host,uint16_t port,bool secure);
bool umi_terminal_remote_remote_endpoint_valid(const UmiTerminalRemoteRemoteEndpoint *value);
#ifdef __cplusplus
}
#endif
#endif
