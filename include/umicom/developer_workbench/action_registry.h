/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/action_registry.h
 *
 * PURPOSE:
 *   Bind product/UI-specific operations to stable Framework command identifiers
 *   without moving those product behaviours into the reusable command catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiDeveloperWorkbenchActionBinding {
    char command_id[UMI_DEVELOPER_WORKBENCH_COMMAND_CAPACITY];
    UmiDeveloperWorkbenchActionHandler handler;
    UmiDeveloperWorkbenchActionEnabled enabled;
    void *user_data;
    uint64_t revision;
} UmiDeveloperWorkbenchActionBinding;

typedef struct UmiDeveloperWorkbenchActionRegistry
    UmiDeveloperWorkbenchActionRegistry;

UmiStatus umi_developer_workbench_action_registry_create(
    UmiDeveloperWorkbenchActionRegistry **out_registry);

void umi_developer_workbench_action_registry_destroy(
    UmiDeveloperWorkbenchActionRegistry *registry);

UmiStatus umi_developer_workbench_action_registry_bind(
    UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    UmiDeveloperWorkbenchActionHandler handler,
    UmiDeveloperWorkbenchActionEnabled enabled,
    void *user_data);

UmiStatus umi_developer_workbench_action_registry_unbind(
    UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id);

UmiStatus umi_developer_workbench_action_registry_find(
    const UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    UmiDeveloperWorkbenchActionBinding *out_binding);

int umi_developer_workbench_action_registry_is_enabled(
    const UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    const char *argument);

UmiStatus umi_developer_workbench_action_registry_execute(
    UmiDeveloperWorkbenchActionRegistry *registry,
    const char *command_id,
    const char *argument,
    char *out_message,
    size_t message_capacity);

size_t umi_developer_workbench_action_registry_count(
    const UmiDeveloperWorkbenchActionRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
