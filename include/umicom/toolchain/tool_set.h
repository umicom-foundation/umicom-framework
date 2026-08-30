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

typedef struct UmiToolchainToolSet {
    uint64_t bits;
} UmiToolchainToolSet;

void umi_toolchain_tool_set_init(UmiToolchainToolSet *set);
UmiStatus umi_toolchain_tool_set_add(UmiToolchainToolSet *set, UmiToolKind kind);
UmiStatus umi_toolchain_tool_set_remove(UmiToolchainToolSet *set, UmiToolKind kind);
int umi_toolchain_tool_set_contains(const UmiToolchainToolSet *set, UmiToolKind kind);
size_t umi_toolchain_tool_set_count(const UmiToolchainToolSet *set);

#ifdef __cplusplus
}
#endif
#endif
