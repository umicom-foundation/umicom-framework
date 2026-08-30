/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_auth_method.h
 *
 * PURPOSE:
 *   Classify remote authentication methods by automation and interactive risk.
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
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiTerminalRemoteRemoteAuthMethodMethod { UMI_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_UNKNOWN=0, UMI_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_AGENT=1, UMI_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_KEY=2, UMI_TERMINAL_REMOTE_REMOTE_AUTH_METHOD_PASSWORD=3 } UmiTerminalRemoteRemoteAuthMethodMethod;
uint32_t umi_terminal_remote_remote_auth_method_risk(UmiTerminalRemoteRemoteAuthMethodMethod method,bool interactive);
#ifdef __cplusplus
}
#endif
#endif
