/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/perspective_registry.h
 *
 * PURPOSE:
 *   Register task-oriented perspectives and track the active work mode.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_REGISTRY_H
#define UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_REGISTRY_H

#include "umicom/developer_workbench/perspective.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperWorkbenchPerspectiveRegistry
    UmiDeveloperWorkbenchPerspectiveRegistry;

UmiStatus umi_developer_workbench_perspective_registry_create(
    UmiDeveloperWorkbenchPerspectiveRegistry **out_registry);

void umi_developer_workbench_perspective_registry_destroy(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry);

UmiStatus umi_developer_workbench_perspective_registry_register(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective);

UmiStatus umi_developer_workbench_perspective_registry_find(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id,
    const UmiDeveloperWorkbenchPerspectiveDefinition **out_perspective);

UmiStatus umi_developer_workbench_perspective_registry_activate(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id);

const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_developer_workbench_perspective_registry_active(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry);

size_t umi_developer_workbench_perspective_registry_count(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
