/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/blame_range.h
 *
 * PURPOSE:
 *   Represent and normalize line ranges requested from blame/history providers.
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
#ifndef UMICOM_VCS_ADVANCED_BLAME_RANGE_H
#define UMICOM_VCS_ADVANCED_BLAME_RANGE_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced blame range data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedBlameRange {
    uint32_t struct_size;
    uint32_t api_version;
    size_t first_line;
    size_t last_line;
} UmiVcsAdvancedBlameRange;

/**
 * Initialise vcs advanced blame range from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_blame_range_init(UmiVcsAdvancedBlameRange *value);
/**
 * Check that vcs advanced blame range satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_blame_range_validate(const UmiVcsAdvancedBlameRange *value);
/**
 * Copy vcs advanced blame range into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_blame_range_set(UmiVcsAdvancedBlameRange *value,
                                             size_t first_line,
                                             size_t last_line);
/**
 * Return the number of records represented by vcs advanced blame range without changing
 * their state.
 */
size_t umi_vcs_advanced_blame_range_count(const UmiVcsAdvancedBlameRange *value);

#ifdef __cplusplus
}
#endif

#endif
