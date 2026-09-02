/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/tool_set.h
 *
 * PURPOSE:
 *   Represent bounded sets of native tool kinds without heap allocation.
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
#ifndef INCLUDE_UMICOM_TOOLCHAIN_TOOL_SET_H
#define INCLUDE_UMICOM_TOOLCHAIN_TOOL_SET_H
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/toolchain/tool.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the toolchain tool set data shared with callers of this public contract.
 */
typedef struct UmiToolchainToolSet {
    uint64_t bits;
} UmiToolchainToolSet;

/**
 * Initialise toolchain tool set from caller-provided values so later operations receive a
 * known state.
 */
void umi_toolchain_tool_set_init(UmiToolchainToolSet *set);
/**
 * Add toolchain tool set only after its inputs and available capacity have been checked.
 */
UmiStatus umi_toolchain_tool_set_add(UmiToolchainToolSet *set, UmiToolKind kind);
/**
 * Remove toolchain tool set while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_toolchain_tool_set_remove(UmiToolchainToolSet *set, UmiToolKind kind);
/**
 * Provide the toolchain tool set contains operation used by this module and its client
 * applications.
 */
int umi_toolchain_tool_set_contains(const UmiToolchainToolSet *set, UmiToolKind kind);
/**
 * Return the number of records represented by toolchain tool set without changing their
 * state.
 */
size_t umi_toolchain_tool_set_count(const UmiToolchainToolSet *set);

#ifdef __cplusplus
}
#endif
#endif
