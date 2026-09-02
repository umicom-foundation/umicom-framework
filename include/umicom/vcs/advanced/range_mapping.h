/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/range_mapping.h
 *
 * PURPOSE:
 *   Map source ranges to destination ranges after edits or diff alignment.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_RANGE_MAPPING_H
#define UMICOM_VCS_ADVANCED_RANGE_MAPPING_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced range mapping data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedRangeMapping {
    uint32_t struct_size;
    uint32_t api_version;
    size_t source_start;
    size_t source_count;
    size_t target_start;
    size_t target_count;
    uint32_t confidence_percent;
} UmiVcsAdvancedRangeMapping;

/**
 * Initialise vcs advanced range mapping from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_range_mapping_init(UmiVcsAdvancedRangeMapping *value);
/**
 * Check that vcs advanced range mapping satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_range_mapping_validate(const UmiVcsAdvancedRangeMapping *value);
/**
 * Provide the vcs advanced range mapping delta operation used by this module and its
 * client applications.
 */
long long umi_vcs_advanced_range_mapping_delta(const UmiVcsAdvancedRangeMapping *value);
/**
 * Provide the vcs advanced range mapping contains source operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_range_mapping_contains_source(const UmiVcsAdvancedRangeMapping *value,
                                                     size_t line);

#ifdef __cplusplus
}
#endif

#endif
