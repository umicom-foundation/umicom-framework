/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/history_cursor.h
 *
 * PURPOSE:
 *   Track deterministic pagination through large repository histories.
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
#ifndef UMICOM_VCS_ADVANCED_HISTORY_CURSOR_H
#define UMICOM_VCS_ADVANCED_HISTORY_CURSOR_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced history cursor data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedHistoryCursor {
    uint32_t struct_size;
    uint32_t api_version;
    size_t offset;
    size_t limit;
    size_t returned;
    size_t total_hint;
    int has_more;
} UmiVcsAdvancedHistoryCursor;

/**
 * Initialise vcs advanced history cursor from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_history_cursor_init(UmiVcsAdvancedHistoryCursor *value);
/**
 * Check that vcs advanced history cursor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_history_cursor_validate(const UmiVcsAdvancedHistoryCursor *value);
/**
 * Provide the vcs advanced history cursor advance operation used by this module and its
 * client applications.
 */
void umi_vcs_advanced_history_cursor_advance(UmiVcsAdvancedHistoryCursor *value,
                                                size_t returned,
                                                int has_more);

#ifdef __cplusplus
}
#endif

#endif
