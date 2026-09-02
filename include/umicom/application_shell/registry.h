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

/**
 * Represent the application shell registry data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationShellRegistry UmiApplicationShellRegistry;

/**
 * Initialise application shell registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_shell_registry_create(
    UmiApplicationShellRegistry **out_registry);

/**
 * Release or reset state held by application shell registry so the same storage can be
 * reused safely.
 */
void umi_application_shell_registry_destroy(
    UmiApplicationShellRegistry *registry);

/**
 * Provide the application shell registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_registry_upsert(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellContribution *contribution);

/**
 * Remove application shell registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_application_shell_registry_remove(
    UmiApplicationShellRegistry *registry,
    const char *contribution_id);

/**
 * Find application shell registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_application_shell_registry_find(
    const UmiApplicationShellRegistry *registry,
    const char *contribution_id,
    UmiApplicationShellContribution *out_contribution);

/**
 * Find application shell registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_application_shell_registry_at(
    const UmiApplicationShellRegistry *registry,
    size_t index,
    UmiApplicationShellContribution *out_contribution);

/**
 * Provide the application shell registry set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_registry_set_state(
    UmiApplicationShellRegistry *registry,
    const char *contribution_id,
    int visible,
    int enabled,
    int checked,
    uint32_t badge_count);

/**
 * Return the number of records represented by application shell registry without changing
 * their state.
 */
size_t umi_application_shell_registry_count(
    const UmiApplicationShellRegistry *registry);

/**
 * Provide the application shell registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_application_shell_registry_revision(
    const UmiApplicationShellRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
