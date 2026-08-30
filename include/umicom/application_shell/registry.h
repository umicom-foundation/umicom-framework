/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application_shell/registry.h
 *
 * PURPOSE:
 *   Store reusable shell contributions in one deterministic Framework-owned
 *   registry so applications do not maintain private menu/panel inventories.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_SHELL_REGISTRY_H
#define UMICOM_APPLICATION_SHELL_REGISTRY_H

#include "umicom/application_shell/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationShellRegistry UmiApplicationShellRegistry;

UmiStatus umi_application_shell_registry_create(
    UmiApplicationShellRegistry **out_registry);

void umi_application_shell_registry_destroy(
    UmiApplicationShellRegistry *registry);

UmiStatus umi_application_shell_registry_upsert(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellContribution *contribution);

UmiStatus umi_application_shell_registry_remove(
    UmiApplicationShellRegistry *registry,
    const char *contribution_id);

UmiStatus umi_application_shell_registry_find(
    const UmiApplicationShellRegistry *registry,
    const char *contribution_id,
    UmiApplicationShellContribution *out_contribution);

UmiStatus umi_application_shell_registry_at(
    const UmiApplicationShellRegistry *registry,
    size_t index,
    UmiApplicationShellContribution *out_contribution);

UmiStatus umi_application_shell_registry_set_state(
    UmiApplicationShellRegistry *registry,
    const char *contribution_id,
    int visible,
    int enabled,
    int checked,
    uint32_t badge_count);

size_t umi_application_shell_registry_count(
    const UmiApplicationShellRegistry *registry);

uint64_t umi_application_shell_registry_revision(
    const UmiApplicationShellRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
