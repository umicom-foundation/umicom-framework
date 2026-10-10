/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/launch_plan.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/launch_plan.h"

#include <stdio.h>
#include <string.h>

/*
 * SOURCE PRESERVATION: The original builder is retained below for review.
 * It populated the caller's plan during traversal and relied on suite helpers
 * having checked member_count, kinds and IDs. A restored/malformed suite can
 * exceed items[], leave a partial plan or carry a duplicate member. The new
 * implementation validates before publication, while keeping the established
 * UNAVAILABLE + complete diagnostic plan when a required member is missing.
 */
#if 0
/*
 * Provide the integration launch plan build operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_launch_plan_build(
    const UmiIntegrationSuiteDefinition *suite,
    const UmiIntegrationRegistry *registry,
    UmiIntegrationLaunchPlan *out_plan)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (suite == NULL || registry == NULL || out_plan == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_plan, 0, sizeof(*out_plan));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < suite->member_count; ++index) {
        const UmiIntegrationSuiteMember *member = &suite->members[index];
        const UmiIntegrationRegistryEntry *entry =
            umi_integration_registry_find_const(
                registry, member->application_id);
        UmiIntegrationLaunchItem *item =
            &out_plan->items[out_plan->count];
        int written = snprintf(item->application_id,
                               sizeof(item->application_id),
                               "%s",
                               member->application_id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (written < 0 ||
            (size_t)written >= sizeof(item->application_id)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }

        item->kind = member->kind;
        item->preferred_frontend = member->preferred_frontend;
        ++out_plan->count;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (entry == NULL) {
            item->disposition =
                member->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED
                    ? UMI_INTEGRATION_LAUNCH_REQUIRED_MISSING
                    : UMI_INTEGRATION_LAUNCH_OPTIONAL_MISSING;
            /* Apply this branch only when its contract condition is satisfied. */
            if (member->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
                ++out_plan->missing_required;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                ++out_plan->missing_optional;
            }
        } else /* Apply this operation only while the related capability or state is available. */ if (!entry->application.enabled) {
            item->disposition = UMI_INTEGRATION_LAUNCH_DISABLED;
            /* Apply this branch only when its contract condition is satisfied. */
            if (member->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED) {
                ++out_plan->missing_required;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                ++out_plan->missing_optional;
            }
        } else /* Apply this branch only when the contract condition is satisfied. */ if (entry->state == UMI_INTEGRATION_APP_RUNNING) {
            item->disposition = UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING;
            ++out_plan->ready_count;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            item->disposition = UMI_INTEGRATION_LAUNCH_READY;
            ++out_plan->ready_count;
        }
    }

    return out_plan->missing_required == 0U
        ? UMI_STATUS_OK
        : UMI_STATUS_UNAVAILABLE;
}
#endif

/* Read exactly the fixed-capacity record owned by Framework. Malformed
 * snapshots must never reach strcmp(), snprintf("%s") or registry lookup. */
static UmiStatus ValidateId(const char *id, size_t capacity)
{
    if (id == NULL || capacity == 0U || id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    return memchr(id, '\0', capacity) == NULL
        ? UMI_STATUS_CAPACITY_EXCEEDED
        : UMI_STATUS_OK;
}

/* Check a deserialised registry's bounds before calling the existing lookup,
 * which assumes bounded, terminated IDs for each registered application. */
static UmiStatus ValidateRegistry(const UmiIntegrationRegistry *registry)
{
    size_t index;
    if (registry->count > UMI_INTEGRATION_MAX_APPLICATIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < registry->count; ++index) {
        UmiStatus status = ValidateId(registry->entries[index].application.id,
                                      sizeof(registry->entries[index].application.id));
        if (status != UMI_STATUS_OK)
            return status;
    }
    return UMI_STATUS_OK;
}

/* Every item is staged in a private complete plan, never in caller output.
 * This retains the normal planning result when required products are absent. */
UmiStatus umi_integration_launch_plan_build(
    const UmiIntegrationSuiteDefinition *suite,
    const UmiIntegrationRegistry *registry,
    UmiIntegrationLaunchPlan *out_plan)
{
    UmiIntegrationLaunchPlan staged = {0};
    UmiStatus status;
    size_t index;

    if (suite == NULL || registry == NULL || out_plan == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = ValidateId(suite->id, sizeof(suite->id));
    if (status != UMI_STATUS_OK)
        return status;
    if (suite->member_count > UMI_INTEGRATION_MAX_MEMBERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = ValidateRegistry(registry);
    if (status != UMI_STATUS_OK)
        return status;

    for (index = 0U; index < suite->member_count; ++index) {
        const UmiIntegrationSuiteMember *member = &suite->members[index];
        const UmiIntegrationRegistryEntry *entry;
        UmiIntegrationLaunchItem *item = &staged.items[index];
        size_t previous;
        size_t id_length;

        status = ValidateId(member->application_id, sizeof(member->application_id));
        if (status != UMI_STATUS_OK)
            return status;
        if (member->kind != UMI_INTEGRATION_DEPENDENCY_REQUIRED &&
            member->kind != UMI_INTEGRATION_DEPENDENCY_OPTIONAL)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (previous = 0U; previous < index; ++previous)
            if (strcmp(staged.items[previous].application_id,
                       member->application_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;

        id_length = strlen(member->application_id);
        (void)memcpy(item->application_id, member->application_id, id_length + 1U);
        item->kind = member->kind;
        item->preferred_frontend = member->preferred_frontend;
        staged.count = index + 1U;
        entry = umi_integration_registry_find_const(registry, item->application_id);
        if (entry == NULL) {
            item->disposition = member->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED
                ? UMI_INTEGRATION_LAUNCH_REQUIRED_MISSING
                : UMI_INTEGRATION_LAUNCH_OPTIONAL_MISSING;
            if (member->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED)
                ++staged.missing_required;
            else
                ++staged.missing_optional;
        } else if (!entry->application.enabled) {
            item->disposition = UMI_INTEGRATION_LAUNCH_DISABLED;
            if (member->kind == UMI_INTEGRATION_DEPENDENCY_REQUIRED)
                ++staged.missing_required;
            else
                ++staged.missing_optional;
        } else if (entry->state == UMI_INTEGRATION_APP_RUNNING) {
            item->disposition = UMI_INTEGRATION_LAUNCH_ALREADY_RUNNING;
            ++staged.ready_count;
        } else {
            item->disposition = UMI_INTEGRATION_LAUNCH_READY;
            ++staged.ready_count;
        }
    }
    /* Publish a complete diagnostic plan, including UNAVAILABLE results. */
    *out_plan = staged;
    return staged.missing_required == 0U ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}

/*
 * Provide the integration launch plan can start operation used by this module and its
 * client applications.
 */
bool umi_integration_launch_plan_can_start(
    const UmiIntegrationLaunchPlan *plan)
{
    return plan != NULL && plan->missing_required == 0U;
}
