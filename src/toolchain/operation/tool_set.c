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

/* Provide the valid kind operation used by this module and its client applications. */
static int valid_kind(UmiToolKind kind)
{
    return kind >= 0 && kind < UMI_TOOL_COUNT && (unsigned)kind < 64U;
}

/*
 * Initialise toolchain tool set from caller-provided values so later operations receive a
 * known state.
 */
void umi_toolchain_tool_set_init(UmiToolchainToolSet *set)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set != NULL) set->bits = 0U;
}

/* Add toolchain tool set only after its inputs and available capacity have been checked. */
UmiStatus umi_toolchain_tool_set_add(UmiToolchainToolSet *set, UmiToolKind kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || !valid_kind(kind)) return UMI_STATUS_INVALID_ARGUMENT;
    set->bits |= (UINT64_C(1) << (unsigned)kind);
    return UMI_STATUS_OK;
}

/*
 * Remove toolchain tool set while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_toolchain_tool_set_remove(UmiToolchainToolSet *set, UmiToolKind kind)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || !valid_kind(kind)) return UMI_STATUS_INVALID_ARGUMENT;
    set->bits &= ~(UINT64_C(1) << (unsigned)kind);
    return UMI_STATUS_OK;
}

/*
 * Provide the toolchain tool set contains operation used by this module and its client
 * applications.
 */
int umi_toolchain_tool_set_contains(const UmiToolchainToolSet *set, UmiToolKind kind)
{
    return set != NULL && valid_kind(kind) &&
        (set->bits & (UINT64_C(1) << (unsigned)kind)) != 0U;
}

/*
 * Return the number of records represented by toolchain tool set without changing their
 * state.
 */
size_t umi_toolchain_tool_set_count(const UmiToolchainToolSet *set)
{
    uint64_t bits;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return 0U;
    bits = set->bits;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (bits != 0U) {
        count += (size_t)(bits & UINT64_C(1));
        bits >>= 1U;
    }
    return count;
}
