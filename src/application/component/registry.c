/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/registry.c
 *
 * PURPOSE:
 *   Implement the single bounded registry used to discover built-in and future
 *   contributed application-component contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/registry.h"

#include <string.h>

void umi_application_component_registry_init(
    UmiApplicationComponentRegistry *registry) {
  if (registry != NULL) {
    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
  }
}

const UmiApplicationComponentContract *umi_application_component_registry_find(
    const UmiApplicationComponentRegistry *registry,
    const char *component_id) {
  size_t index;
  if (registry == NULL || component_id == NULL ||
      registry->count > UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY)
    return NULL;
  for (index = 0U; index < registry->count; ++index) {
    if (strcmp(registry->items[index].definition->component_id, component_id) == 0)
      return &registry->items[index];
  }
  return NULL;
}

UmiStatus umi_application_component_registry_register(
    UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentContract *contract) {
  UmiStatus status;
  if (registry == NULL || contract == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (registry->count > UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  status = umi_application_component_contract_validate(contract);
  if (status != UMI_STATUS_OK)
    return status;
  if (umi_application_component_registry_find(
          registry, contract->definition->component_id) != NULL)
    return UMI_STATUS_ALREADY_EXISTS;
  if (registry->count >= UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  registry->items[registry->count++] = *contract;
  registry->revision += 1U;
  return UMI_STATUS_OK;
}

UmiStatus umi_application_component_registry_seed_catalogue(
    UmiApplicationComponentRegistry *registry) {
  size_t index;
  if (registry == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    UmiApplicationComponentContract contract;
    UmiStatus status = umi_application_component_contract_from_definition(
        umi_application_component_catalogue_at(index), &contract);
    if (status == UMI_STATUS_OK)
      status = umi_application_component_registry_register(registry, &contract);
    if (status != UMI_STATUS_OK)
      return status;
  }
  return UMI_STATUS_OK;
}

const UmiApplicationComponentContract *umi_application_component_registry_at(
    const UmiApplicationComponentRegistry *registry, size_t index) {
  return registry != NULL &&
                 registry->count <= UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY &&
                 index < registry->count
             ? &registry->items[index]
             : NULL;
}

size_t umi_application_component_registry_count(
    const UmiApplicationComponentRegistry *registry) {
  return registry != NULL &&
                 registry->count <= UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY
             ? registry->count
             : 0U;
}

uint64_t umi_application_component_registry_revision(
    const UmiApplicationComponentRegistry *registry) {
  return registry != NULL ? registry->revision : 0U;
}
