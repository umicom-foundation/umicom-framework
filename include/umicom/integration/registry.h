/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/registry.h
 *
 * PURPOSE:
 *   Provide deterministic application discovery without product-to-product coupling.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_REGISTRY_H
#define UMICOM_INTEGRATION_REGISTRY_H

#include "umicom/base/status.h"
#include "umicom/integration/application.h"

typedef struct UmiIntegrationRegistryEntry {
    UmiIntegrationApplication application;
    UmiIntegrationApplicationState state;
} UmiIntegrationRegistryEntry;

typedef struct UmiIntegrationRegistry {
    UmiIntegrationRegistryEntry entries[UMI_INTEGRATION_MAX_APPLICATIONS];
    size_t count;
} UmiIntegrationRegistry;

void umi_integration_registry_init(UmiIntegrationRegistry *registry);
UmiStatus umi_integration_registry_register(
    UmiIntegrationRegistry *registry,
    const UmiIntegrationApplication *application);
UmiStatus umi_integration_registry_unregister(
    UmiIntegrationRegistry *registry,
    const char *application_id);
UmiIntegrationRegistryEntry *umi_integration_registry_find(
    UmiIntegrationRegistry *registry,
    const char *application_id);
const UmiIntegrationRegistryEntry *umi_integration_registry_find_const(
    const UmiIntegrationRegistry *registry,
    const char *application_id);
const UmiIntegrationRegistryEntry *umi_integration_registry_find_capability(
    const UmiIntegrationRegistry *registry,
    const char *capability_id);
UmiStatus umi_integration_registry_set_state(
    UmiIntegrationRegistry *registry,
    const char *application_id,
    UmiIntegrationApplicationState state);

#endif
