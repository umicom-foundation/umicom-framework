/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/capability_registry.h
 *
 * PURPOSE:
 *   Provide reflection-like capability discovery and dependency injection for
 *   statically linked modules, dynamic plug-ins, adapters, services, workers,
 *   agents, applications, and suites while preserving a stable C ABI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_CAPABILITY_REGISTRY_H
#define UMICOM_RUNTIME_CAPABILITY_REGISTRY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/base/version.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CAPABILITY_ID_CAPACITY 128U
#define UMI_CAPABILITY_PROVIDER_CAPACITY 128U
#define UMI_CAPABILITY_REGISTRY_MAX 512U

/**
 * List the named capability flags values accepted by this public contract.
 */
typedef enum UmiCapabilityFlags {
    UMI_CAPABILITY_NONE = 0U,
    UMI_CAPABILITY_SINGLETON = 1U << 0,
    UMI_CAPABILITY_EXTERNAL = 1U << 1,
    UMI_CAPABILITY_RELOAD_SAFE = 1U << 2,
    UMI_CAPABILITY_OPTIONAL = 1U << 3
} UmiCapabilityFlags;

/**
 * Represent the capability descriptor data shared with callers of this public contract.
 */
typedef struct UmiCapabilityDescriptor {
    uint32_t structure_size;
    const char *capability_id;
    UmiVersion capability_version;
    const char *provider_module_id;
    void *provider;
    uint32_t flags;
} UmiCapabilityDescriptor;

/**
 * Represent the capability registry data shared with callers of this public contract.
 */
typedef struct UmiCapabilityRegistry UmiCapabilityRegistry;

/**
 * Initialise capability registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_capability_registry_create(
    UmiCapabilityRegistry **out_registry
);
/**
 * Release or reset state held by capability registry so the same storage can be reused
 * safely.
 */
void umi_capability_registry_destroy(UmiCapabilityRegistry *registry);
/**
 * Add capability registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_capability_registry_register(
    UmiCapabilityRegistry *registry,
    const UmiCapabilityDescriptor *descriptor
);
/**
 * Remove capability registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_capability_registry_unregister(
    UmiCapabilityRegistry *registry,
    const char *capability_id,
    const char *provider_module_id
);
/**
 * Find capability registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCapabilityDescriptor *umi_capability_registry_find(
    const UmiCapabilityRegistry *registry,
    const char *capability_id
);
/**
 * Provide the capability registry find provider operation used by this module and its
 * client applications.
 */
const UmiCapabilityDescriptor *umi_capability_registry_find_provider(
    const UmiCapabilityRegistry *registry,
    const char *capability_id,
    const char *provider_module_id
);
/**
 * Provide the capability registry require operation used by this module and its client
 * applications.
 */
UmiStatus umi_capability_registry_require(
    const UmiCapabilityRegistry *registry,
    const char *const *required_capabilities,
    const char **out_missing_capability
);
/**
 * Return the number of records represented by capability registry without changing their
 * state.
 */
size_t umi_capability_registry_count(
    const UmiCapabilityRegistry *registry
);
/**
 * Find capability registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCapabilityDescriptor *umi_capability_registry_at(
    const UmiCapabilityRegistry *registry,
    size_t index
);

#ifdef __cplusplus
}
#endif

#endif
