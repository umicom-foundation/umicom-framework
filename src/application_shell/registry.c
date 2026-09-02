/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_shell/registry.c
 *
 * PURPOSE:
 *   Implement a bounded, copy-owned registry for reusable shell contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application_shell/registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiApplicationShellRegistry {
    UmiApplicationShellContribution
        items[UMI_APPLICATION_SHELL_MAX_CONTRIBUTIONS];
    size_t count;
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiApplicationShellRegistry *registry,
                         const char *contribution_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || contribution_id == NULL) return (size_t)-1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].contribution_id,
                   contribution_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

/*
 * Initialise application shell registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_shell_registry_create(
    UmiApplicationShellRegistry **out_registry)
{
    UmiApplicationShellRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry =
        (UmiApplicationShellRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by application shell registry so the same storage can be
 * reused safely.
 */
void umi_application_shell_registry_destroy(
    UmiApplicationShellRegistry *registry)
{
    free(registry);
}

/*
 * Provide the application shell registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_registry_upsert(
    UmiApplicationShellRegistry *registry,
    const UmiApplicationShellContribution *contribution)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || contribution == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_application_shell_contribution_validate(contribution);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    index = find_index(registry, contribution->contribution_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_APPLICATION_SHELL_MAX_CONTRIBUTIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }

    registry->items[index] = *contribution;
    registry->items[index].revision = ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove application shell registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_application_shell_registry_remove(
    UmiApplicationShellRegistry *registry,
    const char *contribution_id)
{
    size_t index;
    size_t tail;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || contribution_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, contribution_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    tail = registry->count - index - 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (tail > 0U) {
        (void)memmove(
            &registry->items[index],
            &registry->items[index + 1U],
            tail * sizeof(registry->items[0]));
    }

    registry->count -= 1U;
    (void)memset(&registry->items[registry->count],
                 0,
                 sizeof(registry->items[0]));
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find application shell registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_application_shell_registry_find(
    const UmiApplicationShellRegistry *registry,
    const char *contribution_id,
    UmiApplicationShellContribution *out_contribution)
{
    const size_t index = find_index(registry, contribution_id);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || contribution_id == NULL ||
        out_contribution == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_contribution = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find application shell registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_application_shell_registry_at(
    const UmiApplicationShellRegistry *registry,
    size_t index,
    UmiApplicationShellContribution *out_contribution)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_contribution == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;

    *out_contribution = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the application shell registry set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_shell_registry_set_state(
    UmiApplicationShellRegistry *registry,
    const char *contribution_id,
    int visible,
    int enabled,
    int checked,
    uint32_t badge_count)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || contribution_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, contribution_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (visible) registry->items[index].flags |= UMI_APPLICATION_SHELL_VISIBLE;
    /* Use this fallback path when the earlier condition does not apply. */
    else registry->items[index].flags &= ~(UmiApplicationShellFlags)UMI_APPLICATION_SHELL_VISIBLE;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (enabled) registry->items[index].flags |= UMI_APPLICATION_SHELL_ENABLED;
    /* Use this fallback path when the earlier condition does not apply. */
    else registry->items[index].flags &= ~(UmiApplicationShellFlags)UMI_APPLICATION_SHELL_ENABLED;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (checked) registry->items[index].flags |= UMI_APPLICATION_SHELL_CHECKED;
    /* Use this fallback path when the earlier condition does not apply. */
    else registry->items[index].flags &= ~(UmiApplicationShellFlags)UMI_APPLICATION_SHELL_CHECKED;

    registry->items[index].badge_count = badge_count;
    registry->items[index].revision = ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by application shell registry without changing
 * their state.
 */
size_t umi_application_shell_registry_count(
    const UmiApplicationShellRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the application shell registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_application_shell_registry_revision(
    const UmiApplicationShellRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
