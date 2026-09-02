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

/*
 * Initialise application component registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_component_registry_init(
    UmiApplicationComponentRegistry *registry) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry != NULL) {
    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
  }
}

/*
 * Find application component registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationComponentContract *umi_application_component_registry_find(
    const UmiApplicationComponentRegistry *registry,
    const char *component_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || component_id == NULL ||
      registry->count > UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < registry->count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(registry->items[index].definition->component_id, component_id) == 0)
      return &registry->items[index];
  }
  return NULL;
}

/*
 * Add application component registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_application_component_registry_register(
    UmiApplicationComponentRegistry *registry,
    const UmiApplicationComponentContract *contract) {
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || contract == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (registry->count > UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  status = umi_application_component_contract_validate(contract);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  /* Apply this branch only when its contract condition is satisfied. */
  if (umi_application_component_registry_find(
          registry, contract->definition->component_id) != NULL)
    return UMI_STATUS_ALREADY_EXISTS;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (registry->count >= UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  registry->items[registry->count++] = *contract;
  registry->revision += 1U;
  return UMI_STATUS_OK;
}

/*
 * Provide the application component registry seed catalogue operation used by this module
 * and its client applications.
 */
UmiStatus umi_application_component_registry_seed_catalogue(
    UmiApplicationComponentRegistry *registry) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < umi_application_component_catalogue_count(); ++index) {
    UmiApplicationComponentContract contract;
    UmiStatus status = umi_application_component_contract_from_definition(
        umi_application_component_catalogue_at(index), &contract);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
      status = umi_application_component_registry_register(registry, &contract);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK)
      return status;
  }
  return UMI_STATUS_OK;
}

/*
 * Find application component registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationComponentContract *umi_application_component_registry_at(
    const UmiApplicationComponentRegistry *registry, size_t index) {
  return registry != NULL &&
                 registry->count <= UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY &&
                 index < registry->count
             ? &registry->items[index]
             : NULL;
}

/*
 * Return the number of records represented by application component registry without
 * changing their state.
 */
size_t umi_application_component_registry_count(
    const UmiApplicationComponentRegistry *registry) {
  return registry != NULL &&
                 registry->count <= UMI_APPLICATION_COMPONENT_REGISTRY_CAPACITY
             ? registry->count
             : 0U;
}

/*
 * Provide the application component registry revision operation used by this module and
 * its client applications.
 */
uint64_t umi_application_component_registry_revision(
    const UmiApplicationComponentRegistry *registry) {
  return registry != NULL ? registry->revision : 0U;
}
