/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/action_registry.h
 *
 * PURPOSE:
 *   Bind product/UI-specific operations to stable Framework command identifiers
 *   without moving those product behaviours into the reusable command catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_ACTION_REGISTRY_H
#define UMICOM_DEVELOPER_WORKBENCH_ACTION_REGISTRY_H

#include "umicom/developer_workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiDeveloperWorkbenchActionHandler)(
    void *user_data,
    const char *argument,
    char *out_message,
    size_t message_capacity);

typedef int (*UmiDeveloperWorkbenchActionEnabled)(
    void *user_data,
    const char *argument);

/**
 * Represent the developer workbench action binding data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperWorkbenchActionBinding {
    char command_id[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    UmiDeveloperWorkbenchActionHandler handler;
    UmiDeveloperWorkbenchActionEnabled enabled;
    void *user_data;
    uint64_t revision;
} UmiDeveloperWorkbenchActionBinding;

/**
 * Represent the developer workbench action registry data shared with callers of this
 * public contract.
 */
typedef struct UmiDeveloperWorkbenchActionRegistry
    UmiDeveloperWorkbenchActionRegistry;

/**
 * Initialise developer workbench action registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_workbench_action_registry_create(
    UmiDeveloperWorkbenchActionRegistry **out_registry);

/**
 * Release or reset state held by developer workbench action registry so the same storage
 * can be reused safely.
 */
void umi_developer_workbench_action_registry_destroy(
    UmiDeveloperWorkbenchActionRegistry *registry);

/**
 * Provide the developer workbench action registry bind operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_action_registry_bind(
    UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    UmiDeveloperWorkbenchActionHandler handler,
    UmiDeveloperWorkbenchActionEnabled enabled,
    void *user_data);

/**
 * Provide the developer workbench action registry unbind operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_workbench_action_registry_unbind(
    UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id);

/**
 * Find developer workbench action registry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_developer_workbench_action_registry_find(
    const UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    UmiDeveloperWorkbenchActionBinding *out_binding);

/**
 * Provide the developer workbench action registry is enabled operation used by this module
 * and its client applications.
 */
int umi_developer_workbench_action_registry_is_enabled(
    const UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    const char *argument);

/**
 * Perform developer workbench action registry through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_developer_workbench_action_registry_execute(
    UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

/**
 * Return the number of records represented by developer workbench action registry without
 * changing their state.
 */
size_t umi_developer_workbench_action_registry_count(
    const UmiDeveloperWorkbenchActionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
