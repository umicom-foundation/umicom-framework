/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/component/registry.h
 *
 * PURPOSE:
 *   Register, discover and resolve versioned component contracts through one
 *   application-neutral Framework registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_COMPONENT_REGISTRY_H
#define UMICOM_APPLICATION_COMPONENT_REGISTRY_H

#include "umicom/application/component/contract.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationComponentRegistry {
  UmiApplicationComponentContract
      items[UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY];
  size_t count;
  uint64_t revision;
} UmiApplicationComponentRegistry;

void umi_application_component_registry_init(
    UmiApplicationComponentRegistry *registry);
UmiStatus umi_application_component_registry_register(
    UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentContract *contract);
UmiStatus umi_application_component_registry_seed_catalogue(
    UmiApplicationComponentRegistry *registry);
const UmiApplicationComponentContract *umi_application_component_registry_find(
    const UmiApplicationComponentRegistry *registry,
    const char *component_id);
const UmiApplicationComponentContract *umi_application_component_registry_at(
    const UmiApplicationComponentRegistry *registry, size_t index);
size_t umi_application_component_registry_count(
    const UmiApplicationComponentRegistry *registry);
uint64_t umi_application_component_registry_revision(
    const UmiApplicationComponentRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
