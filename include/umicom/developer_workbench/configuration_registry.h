/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/configuration_registry.h
 *
 * PURPOSE:
 *   Maintain reusable named developer lifecycle configurations and active state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_CONFIGURATION_REGISTRY_H
#define UMICOM_DEVELOPER_WORKBENCH_CONFIGURATION_REGISTRY_H

#include "umicom/developer_workbench/configuration.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer workbench configuration registry data shared with callers of
 * this public contract.
 */
typedef struct UmiDeveloperWorkbenchConfigurationRegistry
    UmiDeveloperWorkbenchConfigurationRegistry;

/**
 * Initialise developer workbench configuration registry from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_developer_workbench_configuration_registry_create(
    UmiDeveloperWorkbenchConfigurationRegistry **out_registry);

/**
 * Release or reset state held by developer workbench configuration registry so the same
 * storage can be reused safely.
 */
void umi_developer_workbench_configuration_registry_destroy(
    UmiDeveloperWorkbenchConfigurationRegistry *registry);

/**
 * Provide the developer workbench configuration registry upsert operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_configuration_registry_upsert(
    UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const UmiDeveloperWorkbenchConfiguration *configuration);

/**
 * Find developer workbench configuration registry while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_developer_workbench_configuration_registry_find(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id,
    UmiDeveloperWorkbenchConfiguration *out_configuration);

/**
 * Find developer workbench configuration registry while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_developer_workbench_configuration_registry_at(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    size_t index,
    UmiDeveloperWorkbenchConfiguration *out_configuration);

/**
 * Provide the developer workbench configuration registry activate operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_configuration_registry_activate(
    UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id);

/**
 * Provide the developer workbench configuration registry active operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_configuration_registry_active(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    UmiDeveloperWorkbenchConfiguration *out_configuration);

/**
 * Return the number of records represented by developer workbench configuration registry
 * without changing their state.
 */
size_t umi_developer_workbench_configuration_registry_count(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry);

/**
 * Provide the developer workbench configuration registry revision operation used by this
 * module and its client applications.
 */
uint64_t umi_developer_workbench_configuration_registry_revision(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
