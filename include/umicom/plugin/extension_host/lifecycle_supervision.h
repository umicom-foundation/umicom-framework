/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/lifecycle_supervision.h
 *
 * PURPOSE:
 *   Choose lifecycle supervision actions following crash, timeout or policy failure.
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
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_LIFECYCLE_SUPERVISION_H
#define UMICOM_PLUGIN_EXTENSION_HOST_LIFECYCLE_SUPERVISION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiPluginExtensionHostLifecycleSupervision { uint32_t crash_count; uint32_t restart_count; uint32_t maximum_restarts; int trust_failure; int policy_failure; int heartbeat_timeout; } UmiPluginExtensionHostLifecycleSupervision;
UmiPluginExtensionHostSupervisionAction umi_plugin_extension_host_lifecycle_supervision_decide(const UmiPluginExtensionHostLifecycleSupervision *value);

#ifdef __cplusplus
}
#endif

#endif
