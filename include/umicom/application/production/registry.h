/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/production/registry.h
 *
 * PURPOSE:
 *   Publish one bounded contract in the Framework-owned application production
 *   control plane without moving business logic into the Master Controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTION_REGISTRY_H
#define UMICOM_APPLICATION_PRODUCTION_REGISTRY_H

#include "umicom/application/production/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/application/production/application_binding.h"

typedef struct UmiApplicationProductionRegistry {
    UmiApplicationProductionBinding
        entries[UMI_APPLICATION_PRODUCTION_MAX_APPLICATIONS];
    size_t count;
} UmiApplicationProductionRegistry;

void umi_application_production_registry_init(
    UmiApplicationProductionRegistry *registry);
UmiStatus umi_application_production_registry_register(
    UmiApplicationProductionRegistry *registry,
    const UmiProductApplicationAdoption *adoption);
const UmiApplicationProductionBinding *
umi_application_production_registry_find(
    const UmiApplicationProductionRegistry *registry,
    const char *application_id);

#ifdef __cplusplus
}
#endif
#endif
