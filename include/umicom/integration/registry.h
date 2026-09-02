/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/registry.h
 *
 * PURPOSE:
 *   Provide deterministic application discovery without product-to-product coupling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_REGISTRY_H
#define UMICOM_INTEGRATION_REGISTRY_H

#include "umicom/base/status.h"
#include "umicom/integration/application.h"

/**
 * Represent the integration registry entry data shared with callers of this public
 * contract.
 */
typedef struct UmiIntegrationRegistryEntry {
    UmiIntegrationApplication application;
    UmiIntegrationApplicationState state;
} UmiIntegrationRegistryEntry;

/**
 * Represent the integration registry data shared with callers of this public contract.
 */
typedef struct UmiIntegrationRegistry {
    UmiIntegrationRegistryEntry entries[UMI_INTEGRATION_MAX_APPLICATIONS];
    size_t count;
} UmiIntegrationRegistry;

/**
 * Initialise integration registry from caller-provided values so later operations receive
 * a known state.
 */
void umi_integration_registry_init(UmiIntegrationRegistry *registry);
/**
 * Add integration registry only after its inputs and available capacity have been checked.
 */
UmiStatus umi_integration_registry_register(
    UmiIntegrationRegistry *registry,
    const UmiIntegrationApplication *application);
/**
 * Remove integration registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_integration_registry_unregister(
    UmiIntegrationRegistry *registry,
    const char *application_id);
/**
 * Find integration registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiIntegrationRegistryEntry *umi_integration_registry_find(
    UmiIntegrationRegistry *registry,
    const char *application_id);
/**
 * Provide the integration registry find const operation used by this module and its client
 * applications.
 */
const UmiIntegrationRegistryEntry *umi_integration_registry_find_const(
    const UmiIntegrationRegistry *registry,
    const char *application_id);
/**
 * Provide the integration registry find capability operation used by this module and its
 * client applications.
 */
const UmiIntegrationRegistryEntry *umi_integration_registry_find_capability(
    const UmiIntegrationRegistry *registry,
    const char *capability_id);
/**
 * Provide the integration registry set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_registry_set_state(
    UmiIntegrationRegistry *registry,
    const char *application_id,
    UmiIntegrationApplicationState state);

#endif
