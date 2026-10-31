/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_gateway_discovery.h
 *
 * PURPOSE:
 *   Discover installed Gateway launchers without reading account configuration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_IBKR_GATEWAY_DISCOVERY_H
#define UMICOM_IBKR_GATEWAY_DISCOVERY_H
#include <gio/gio.h>
/* Roots are directories, not search patterns. NULL uses known locations.
 * An explicit path takes precedence and fails visibly if it is not a launcher.
 * Results are owned by the caller; credentials and configuration are not read. */
gboolean UmiIbkrGatewayDiscover(const char *const *roots, const char *custom,
    GCancellable *cancel, char ***outPaths, gboolean *outIncomplete, GError **error);
gboolean UmiIbkrGatewayValidate(const char *path, GCancellable *cancel, GError **error);
gboolean UmiIbkrGatewayLaunch(const char *path, GCancellable *cancel, GError **error);
#endif
