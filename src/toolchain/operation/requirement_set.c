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

void umi_toolchain_requirement_set_init(UmiToolchainRequirementSet *set)
{
    if (set != NULL) (void)memset(set, 0, sizeof(*set));
}

const UmiToolchainRequirement *umi_toolchain_requirement_set_find(
    const UmiToolchainRequirementSet *set, UmiToolKind kind)
{
    size_t index;
    if (set == NULL) return NULL;
    for (index = 0U; index < set->count; ++index) {
        if (set->items[index].kind == kind) return &set->items[index];
    }
    return NULL;
}

int umi_toolchain_requirement_set_contains(const UmiToolchainRequirementSet *set,
                                           UmiToolKind kind)
{
    return umi_toolchain_requirement_set_find(set, kind) != NULL;
}

UmiStatus umi_toolchain_requirement_set_add(UmiToolchainRequirementSet *set,
                                            UmiToolKind kind,
                                            int required)
{
    UmiToolchainRequirement requirement;
    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_toolchain_requirement_set_contains(set, kind)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (set->count >= UMI_TOOLCHAIN_REQUIREMENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    umi_toolchain_requirement_init(&requirement, kind, required);
    if (umi_toolchain_requirement_validate(&requirement) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    set->items[set->count++] = requirement;
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_requirement_set_validate(const UmiToolchainRequirementSet *set)
{
    size_t index;
    size_t other;
    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (set->count > UMI_TOOLCHAIN_REQUIREMENT_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (index = 0U; index < set->count; ++index) {
        if (umi_toolchain_requirement_validate(&set->items[index]) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        for (other = index + 1U; other < set->count; ++other) {
            if (set->items[index].kind == set->items[other].kind) {
                return UMI_STATUS_ALREADY_EXISTS;
            }
        }
    }
    return UMI_STATUS_OK;
}
