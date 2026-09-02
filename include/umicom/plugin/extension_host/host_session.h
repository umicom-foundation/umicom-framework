/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/host_session.h
 *
 * PURPOSE:
 *   Track one extension-host session through connect, ready, draining and closed states.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_HOST_SESSION_H
#define UMICOM_PLUGIN_EXTENSION_HOST_HOST_SESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named plugin extension host host session state values accepted by this public
 * contract.
 */
typedef enum UmiPluginExtensionHostHostSessionState { UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DISCONNECTED=0, UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CONNECTING=1, UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_READY=2, UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_DRAINING=3, UMI_PLUGIN_EXTENSION_HOST_HOST_SESSION_CLOSED=4 } UmiPluginExtensionHostHostSessionState;
/**
 * Represent the plugin extension host host session data shared with callers of this public
 * contract.
 */
typedef struct UmiPluginExtensionHostHostSession { char id[UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY]; UmiPluginExtensionHostHostSessionState state; uint64_t opened_ms; uint64_t revision; } UmiPluginExtensionHostHostSession;
/**
 * Initialise plugin extension host host session from caller-provided values so later
 * operations receive a known state.
 */
void umi_plugin_extension_host_host_session_init(UmiPluginExtensionHostHostSession *session, const char *id);
/**
 * Provide the plugin extension host host session transition operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_host_host_session_transition(UmiPluginExtensionHostHostSession *session, UmiPluginExtensionHostHostSessionState next);
/**
 * Provide the plugin extension host host session ready operation used by this module and
 * its client applications.
 */
int umi_plugin_extension_host_host_session_ready(const UmiPluginExtensionHostHostSession *session);

#ifdef __cplusplus
}
#endif

#endif
