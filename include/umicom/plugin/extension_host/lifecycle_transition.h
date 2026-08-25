/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/lifecycle_transition.h
 *
 * PURPOSE:
 *   Validate explicit extension lifecycle transitions.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_LIFECYCLE_TRANSITION_H
#define UMICOM_PLUGIN_EXTENSION_HOST_LIFECYCLE_TRANSITION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

int umi_plugin_extension_host_lifecycle_transition_allowed(UmiPluginExtensionHostLifecycle current, UmiPluginExtensionHostLifecycle next);
UmiStatus umi_plugin_extension_host_lifecycle_transition_validate(UmiPluginExtensionHostLifecycle current, UmiPluginExtensionHostLifecycle next);

#ifdef __cplusplus
}
#endif

#endif
