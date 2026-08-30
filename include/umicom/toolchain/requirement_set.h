/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/toolchain/requirement_set.h
 *
 * PURPOSE:
 *   Store deterministic operation-specific native-tool requirements.
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
#ifndef INCLUDE_UMICOM_TOOLCHAIN_REQUIREMENT_SET_H
#define INCLUDE_UMICOM_TOOLCHAIN_REQUIREMENT_SET_H
#include <stddef.h>
#include "umicom/toolchain/requirement.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TOOLCHAIN_REQUIREMENT_CAPACITY 32U

typedef struct UmiToolchainRequirementSet {
    UmiToolchainRequirement items[UMI_TOOLCHAIN_REQUIREMENT_CAPACITY];
    size_t count;
} UmiToolchainRequirementSet;

void umi_toolchain_requirement_set_init(UmiToolchainRequirementSet *set);
UmiStatus umi_toolchain_requirement_set_add(UmiToolchainRequirementSet *set,
                                            UmiToolKind kind,
                                            int required);
const UmiToolchainRequirement *umi_toolchain_requirement_set_find(
    const UmiToolchainRequirementSet *set, UmiToolKind kind);
int umi_toolchain_requirement_set_contains(const UmiToolchainRequirementSet *set,
                                           UmiToolKind kind);
UmiStatus umi_toolchain_requirement_set_validate(const UmiToolchainRequirementSet *set);

#ifdef __cplusplus
}
#endif
#endif
