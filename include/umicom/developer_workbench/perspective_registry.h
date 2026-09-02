/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/perspective_registry.h
 *
 * PURPOSE:
 *   Register task-oriented perspectives and track the active work mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_REGISTRY_H
#define UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_REGISTRY_H

#include "umicom/developer_workbench/perspective.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer workbench perspective registry data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperWorkbenchPerspectiveRegistry
    UmiDeveloperWorkbenchPerspectiveRegistry;

/**
 * Initialise developer workbench perspective registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_workbench_perspective_registry_create(
    UmiDeveloperWorkbenchPerspectiveRegistry **out_registry);

/**
 * Release or reset state held by developer workbench perspective registry so the same
 * storage can be reused safely.
 */
void umi_developer_workbench_perspective_registry_destroy(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry);

/**
 * Add developer workbench perspective registry only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_developer_workbench_perspective_registry_register(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective);

/**
 * Find developer workbench perspective registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_developer_workbench_perspective_registry_find(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id,
    const UmiDeveloperWorkbenchPerspectiveDefinition **out_perspective);

/**
 * Provide the developer workbench perspective registry activate operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_perspective_registry_activate(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id);

/**
 * Provide the developer workbench perspective registry active operation used by this
 * module and its client applications.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_developer_workbench_perspective_registry_active(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry);

/**
 * Return the number of records represented by developer workbench perspective registry
 * without changing their state.
 */
size_t umi_developer_workbench_perspective_registry_count(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
