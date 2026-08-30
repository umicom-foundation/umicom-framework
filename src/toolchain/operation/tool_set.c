/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/tool_set.c
 *
 * PURPOSE:
 *   Implement compact tool-set operations used by scoped discovery policies.
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
#include "umicom/toolchain/tool_set.h"

static int valid_kind(UmiToolKind kind)
{
    return kind >= 0 && kind < UMI_TOOL_COUNT && (unsigned)kind < 64U;
}

void umi_toolchain_tool_set_init(UmiToolchainToolSet *set)
{
    if (set != NULL) set->bits = 0U;
}

UmiStatus umi_toolchain_tool_set_add(UmiToolchainToolSet *set, UmiToolKind kind)
{
    if (set == NULL || !valid_kind(kind)) return UMI_STATUS_INVALID_ARGUMENT;
    set->bits |= (UINT64_C(1) << (unsigned)kind);
    return UMI_STATUS_OK;
}

UmiStatus umi_toolchain_tool_set_remove(UmiToolchainToolSet *set, UmiToolKind kind)
{
    if (set == NULL || !valid_kind(kind)) return UMI_STATUS_INVALID_ARGUMENT;
    set->bits &= ~(UINT64_C(1) << (unsigned)kind);
    return UMI_STATUS_OK;
}

int umi_toolchain_tool_set_contains(const UmiToolchainToolSet *set, UmiToolKind kind)
{
    return set != NULL && valid_kind(kind) &&
        (set->bits & (UINT64_C(1) << (unsigned)kind)) != 0U;
}

size_t umi_toolchain_tool_set_count(const UmiToolchainToolSet *set)
{
    uint64_t bits;
    size_t count = 0U;
    if (set == NULL) return 0U;
    bits = set->bits;
    while (bits != 0U) {
        count += (size_t)(bits & UINT64_C(1));
        bits >>= 1U;
    }
    return count;
}
