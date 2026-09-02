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

/**
 * Represent the application component registry data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationComponentRegistry {
  UmiApplicationComponentContract
      items[UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY];
  size_t count;
  uint64_t revision;
} UmiApplicationComponentRegistry;

/**
 * Initialise application component registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_component_registry_init(
    UmiApplicationComponentRegistry *registry);
/**
 * Add application component registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_application_component_registry_register(
    UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentContract *contract);
/**
 * Provide the application component registry seed catalogue operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_component_registry_seed_catalogue(
    UmiApplicationComponentRegistry *registry);
/**
 * Find application component registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationComponentContract *umi_application_component_registry_find(
    const UmiApplicationComponentRegistry *registry,
    const char *component_id);
/**
 * Find application component registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationComponentContract *umi_application_component_registry_at(
    const UmiApplicationComponentRegistry *registry, size_t index);
/**
 * Return the number of records represented by application component registry without
 * changing their state.
 */
size_t umi_application_component_registry_count(
    const UmiApplicationComponentRegistry *registry);
/**
 * Provide the application component registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_application_component_registry_revision(
    const UmiApplicationComponentRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
