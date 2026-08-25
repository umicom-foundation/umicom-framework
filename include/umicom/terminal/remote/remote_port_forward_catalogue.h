/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/remote/remote_port_forward_catalogue.h
 *
 * PURPOSE:
 *   Manage bounded remote port forward catalogue entries with deterministic lookup and duplicate rejection.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_REMOTE_REMOTE_PORT_FORWARD_CATALOGUE_H
#define UMICOM_TERMINAL_REMOTE_REMOTE_PORT_FORWARD_CATALOGUE_H
#include "umicom/terminal/remote/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTerminalRemoteRemotePortForwardCatalogue { UmiTerminalRemoteNamedEntry items[UMI_TERMINAL_REMOTE_CATALOGUE_CAPACITY]; size_t count; uint64_t revision; } UmiTerminalRemoteRemotePortForwardCatalogue;
void umi_terminal_remote_remote_port_forward_catalogue_init(UmiTerminalRemoteRemotePortForwardCatalogue *value);
UmiStatus umi_terminal_remote_remote_port_forward_catalogue_add(UmiTerminalRemoteRemotePortForwardCatalogue *value, const char *id, const char *label);
const UmiTerminalRemoteNamedEntry *umi_terminal_remote_remote_port_forward_catalogue_find(const UmiTerminalRemoteRemotePortForwardCatalogue *value, const char *id);
size_t umi_terminal_remote_remote_port_forward_catalogue_enabled_count(const UmiTerminalRemoteRemotePortForwardCatalogue *value);
#ifdef __cplusplus
}
#endif
#endif
