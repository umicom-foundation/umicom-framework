/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/requirement_set.c
 *
 * PURPOSE:
 *   Implement duplicate-safe requirement sets for native operations.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/requirement_set.h"

#include <string.h>

/*
 * Initialise toolchain requirement set from caller-provided values so later operations
 * receive a known state.
 */
void umi_toolchain_requirement_set_init(UmiToolchainRequirementSet *set)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set != NULL) (void)memset(set, 0, sizeof(*set));
}

/*
 * Find toolchain requirement set while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiToolchainRequirement *umi_toolchain_requirement_set_find(
    const UmiToolchainRequirementSet *set, UmiToolKind kind)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (set->items[index].kind == kind) return &set->items[index];
    }
    return NULL;
}

/*
 * Provide the toolchain requirement set contains operation used by this module and its
 * client applications.
 */
int umi_toolchain_requirement_set_contains(const UmiToolchainRequirementSet *set,
                                           UmiToolKind kind)
{
    return umi_toolchain_requirement_set_find(set, kind) != NULL;
}

/*
 * Add toolchain requirement set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_toolchain_requirement_set_add(UmiToolchainRequirementSet *set,
                                            UmiToolKind kind,
                                            int required)
{
    UmiToolchainRequirement requirement;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_toolchain_requirement_set_contains(set, kind)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_TOOLCHAIN_REQUIREMENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    umi_toolchain_requirement_init(&requirement, kind, required);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_toolchain_requirement_validate(&requirement) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    set->items[set->count++] = requirement;
    return UMI_STATUS_OK;
}

/*
 * Check that toolchain requirement set satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_toolchain_requirement_set_validate(const UmiToolchainRequirementSet *set)
{
    size_t index;
    size_t other;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count > UMI_TOOLCHAIN_REQUIREMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_toolchain_requirement_validate(&set->items[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (other = index + 1U; other < set->count; ++other) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (set->items[index].kind == set->items[other].kind) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    return UMI_STATUS_OK;
}
