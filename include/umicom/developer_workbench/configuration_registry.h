/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/configuration_registry.h
 *
 * PURPOSE:
 *   Maintain reusable named developer lifecycle configurations and active state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_CONFIGURATION_REGISTRY_H
#define UMICOM_DEVELOPER_WORKBENCH_CONFIGURATION_REGISTRY_H

#include "umicom/developer_workbench/configuration.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperWorkbenchConfigurationRegistry
    UmiDeveloperWorkbenchConfigurationRegistry;

UmiStatus umi_developer_workbench_configuration_registry_create(
    UmiDeveloperWorkbenchConfigurationRegistry **out_registry);

void umi_developer_workbench_configuration_registry_destroy(
    UmiDeveloperWorkbenchConfigurationRegistry *registry);

UmiStatus umi_developer_workbench_configuration_registry_upsert(
    UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const UmiDeveloperWorkbenchConfiguration *configuration);

UmiStatus umi_developer_workbench_configuration_registry_find(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id,
    UmiDeveloperWorkbenchConfiguration *out_configuration);

UmiStatus umi_developer_workbench_configuration_registry_at(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    size_t index,
    UmiDeveloperWorkbenchConfiguration *out_configuration);

UmiStatus umi_developer_workbench_configuration_registry_activate(
    UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id);

UmiStatus umi_developer_workbench_configuration_registry_active(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    UmiDeveloperWorkbenchConfiguration *out_configuration);

size_t umi_developer_workbench_configuration_registry_count(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry);

uint64_t umi_developer_workbench_configuration_registry_revision(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
