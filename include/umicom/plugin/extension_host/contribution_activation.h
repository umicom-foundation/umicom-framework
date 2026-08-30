/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/contribution_activation.h
 *
 * PURPOSE:
 *   Evaluate whether a contribution may activate in the current host state.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_CONTRIBUTION_ACTIVATION_H
#define UMICOM_PLUGIN_EXTENSION_HOST_CONTRIBUTION_ACTIVATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostContributionActivation { int plugin_enabled; int dependency_ready; int permission_ready; int extension_point_ready; int quarantined; uint32_t risk; } UmiPluginExtensionHostContributionActivation;
UmiPluginExtensionHostDecision umi_plugin_extension_host_contribution_activation_evaluate(const UmiPluginExtensionHostContributionActivation *value);

#ifdef __cplusplus
}
#endif

#endif
