/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_port_forward.h
 *
 * PURPOSE:
 *   Validate local-to-remote TCP port-forward contracts.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_PORT_FORWARD_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_PORT_FORWARD_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemotePortForward { uint16_t local_port; uint16_t remote_port; char remote_host[UMI_TERMINAL_REMOTE_TEXT_CAPACITY]; bool enabled; } UmiTerminalRemoteRemotePortForward;
void umi_terminal_remote_remote_port_forward_init(UmiTerminalRemoteRemotePortForward *value,uint16_t local_port,const char *remote_host,uint16_t remote_port);
bool umi_terminal_remote_remote_port_forward_valid(const UmiTerminalRemoteRemotePortForward *value);
#ifdef __cplusplus
}
#endif
#endif
