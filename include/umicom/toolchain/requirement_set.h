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

/**
 * Represent the toolchain requirement set data shared with callers of this public
 * contract.
 */
typedef struct UmiToolchainRequirementSet {
    UmiToolchainRequirement items[UMI_TOOLCHAIN_REQUIREMENT_CAPACITY];
    size_t count;
} UmiToolchainRequirementSet;

/**
 * Initialise toolchain requirement set from caller-provided values so later operations
 * receive a known state.
 */
void umi_toolchain_requirement_set_init(UmiToolchainRequirementSet *set);
/**
 * Add toolchain requirement set only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_toolchain_requirement_set_add(UmiToolchainRequirementSet *set,
                                            UmiToolKind kind,
                                            int required);
/**
 * Find toolchain requirement set while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiToolchainRequirement *umi_toolchain_requirement_set_find(
    const UmiToolchainRequirementSet *set, UmiToolKind kind);
/**
 * Provide the toolchain requirement set contains operation used by this module and its
 * client applications.
 */
int umi_toolchain_requirement_set_contains(const UmiToolchainRequirementSet *set,
                                           UmiToolKind kind);
/**
 * Check that toolchain requirement set satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_toolchain_requirement_set_validate(const UmiToolchainRequirementSet *set);

#ifdef __cplusplus
}
#endif
#endif
