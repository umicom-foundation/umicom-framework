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
/**
 * Represent the terminal remote remote port forward data shared with callers of this
 * public contract.
 */
typedef struct UmiTerminalRemoteRemotePortForward { uint16_t local_port; uint16_t remote_port; char remote_host[UMI_TERMINAL_REMOTE_TEXT_CAPACITY]; bool enabled; } UmiTerminalRemoteRemotePortForward;
/**
 * Initialise terminal remote remote port forward from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_port_forward_init(UmiTerminalRemoteRemotePortForward *value,uint16_t local_port,const char *remote_host,uint16_t remote_port);
/**
 * Check that terminal remote remote port forward satisfies its contract before another
 * service relies on it.
 */
bool umi_terminal_remote_remote_port_forward_valid(const UmiTerminalRemoteRemotePortForward *value);
#ifdef __cplusplus
}
#endif
#endif
