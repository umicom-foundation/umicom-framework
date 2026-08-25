/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_host/types.h
 *
 * PURPOSE:
 *   Define shared stable states, decisions, capacities and evidence helpers for the isolated plug-in extension host.
 *
 * ARCHITECTURE:
 *   Umicom Framework owns extension contracts, trust, isolation and lifecycle.
 *   Studio, Desk and every product remain thin consumers of these services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_HOST_TYPES_H
#define UMICOM_PLUGIN_EXTENSION_HOST_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/plugin/extension_host/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLUGIN_EXTENSION_HOST_API_VERSION 1U
#define UMI_PLUGIN_EXTENSION_HOST_ID_CAPACITY 128U
#define UMI_PLUGIN_EXTENSION_HOST_TEXT_CAPACITY 256U
#define UMI_PLUGIN_EXTENSION_HOST_SMALL_CAPACITY 32U
#define UMI_PLUGIN_EXTENSION_HOST_MEDIUM_CAPACITY 64U
#define UMI_PLUGIN_EXTENSION_HOST_LARGE_CAPACITY 128U

typedef enum UmiPluginExtensionHostDecision {
    UMI_PLUGIN_EXTENSION_HOST_DENY = 0,
    UMI_PLUGIN_EXTENSION_HOST_REQUIRE_APPROVAL = 1,
    UMI_PLUGIN_EXTENSION_HOST_ALLOW = 2
} UmiPluginExtensionHostDecision;

typedef enum UmiPluginExtensionHostTrust {
    UMI_PLUGIN_EXTENSION_HOST_TRUST_UNTRUSTED = 0,
    UMI_PLUGIN_EXTENSION_HOST_TRUST_UNKNOWN = 1,
    UMI_PLUGIN_EXTENSION_HOST_TRUST_TRUSTED = 2,
    UMI_PLUGIN_EXTENSION_HOST_TRUST_SYSTEM = 3
} UmiPluginExtensionHostTrust;

typedef enum UmiPluginExtensionHostIsolation {
    UMI_PLUGIN_EXTENSION_HOST_ISOLATION_IN_PROCESS = 0,
    UMI_PLUGIN_EXTENSION_HOST_ISOLATION_PROCESS = 1,
    UMI_PLUGIN_EXTENSION_HOST_ISOLATION_RESTRICTED_PROCESS = 2
} UmiPluginExtensionHostIsolation;

typedef enum UmiPluginExtensionHostHealthState {
    UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNKNOWN = 0,
    UMI_PLUGIN_EXTENSION_HOST_HEALTH_HEALTHY = 1,
    UMI_PLUGIN_EXTENSION_HOST_HEALTH_DEGRADED = 2,
    UMI_PLUGIN_EXTENSION_HOST_HEALTH_UNHEALTHY = 3
} UmiPluginExtensionHostHealthState;

typedef enum UmiPluginExtensionHostLifecycle {
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_DISCOVERED = 0,
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_VERIFIED = 1,
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_INSTALLED = 2,
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_ENABLED = 3,
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STARTING = 4,
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_RUNNING = 5,
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPING = 6,
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_STOPPED = 7,
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_FAILED = 8,
    UMI_PLUGIN_EXTENSION_HOST_LIFECYCLE_QUARANTINED = 9
} UmiPluginExtensionHostLifecycle;

typedef enum UmiPluginExtensionHostSupervisionAction {
    UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_NONE = 0,
    UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_RESTART = 1,
    UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_DISABLE = 2,
    UMI_PLUGIN_EXTENSION_HOST_SUPERVISION_QUARANTINE = 3
} UmiPluginExtensionHostSupervisionAction;

typedef uint64_t UmiPluginExtensionHostCapabilityFlags;

uint64_t umi_plugin_extension_host_hash_bytes(const void *data, size_t size);
uint64_t umi_plugin_extension_host_hash_text(const char *text);
int umi_plugin_extension_host_text_valid(const char *text, size_t capacity);
const char *umi_plugin_extension_host_decision_text(UmiPluginExtensionHostDecision decision);

#ifdef __cplusplus
}
#endif

#endif
