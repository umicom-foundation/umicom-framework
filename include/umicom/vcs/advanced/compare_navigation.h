/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/compare_navigation.h
 *
 * PURPOSE:
 *   Track deterministic next/previous change navigation in comparison sessions.
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
#ifndef UMICOM_VCS_ADVANCED_COMPARE_NAVIGATION_H
#define UMICOM_VCS_ADVANCED_COMPARE_NAVIGATION_H

#include "umicom/vcs/advanced/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced compare navigation data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedCompareNavigation {
    uint32_t struct_size;
    uint32_t api_version;
    size_t change_count;
    size_t current_index;
    int wrap;
} UmiVcsAdvancedCompareNavigation;

/**
 * Initialise vcs advanced compare navigation from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_compare_navigation_init(UmiVcsAdvancedCompareNavigation *value);
/**
 * Check that vcs advanced compare navigation satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_compare_navigation_validate(const UmiVcsAdvancedCompareNavigation *value);
/**
 * Provide the vcs advanced compare navigation next operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_compare_navigation_next(UmiVcsAdvancedCompareNavigation *value);
/**
 * Provide the vcs advanced compare navigation previous operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_compare_navigation_previous(UmiVcsAdvancedCompareNavigation *value);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_vcs_advanced_compare_navigation_archive_encode(const UmiVcsAdvancedCompareNavigation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_vcs_advanced_compare_navigation_archive_decode(const void *bytes, size_t byte_count,
    UmiVcsAdvancedCompareNavigation *value);

#ifdef __cplusplus
}
#endif

#endif
