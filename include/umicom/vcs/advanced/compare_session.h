/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/compare_session.h
 *
 * PURPOSE:
 *   Manage professional two-way/three-way comparison session identity and synchronized view state.
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
#ifndef UMICOM_VCS_ADVANCED_COMPARE_SESSION_H
#define UMICOM_VCS_ADVANCED_COMPARE_SESSION_H
#include "umicom/vcs/advanced/compare_side.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced compare session data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedCompareSession {
    uint32_t struct_size; uint32_t api_version;
    char session_id[UMI_VCS_ADVANCED_ID_CAPACITY]; UmiVcsCompareMode mode;
    UmiVcsAdvancedCompareSide left,right,base; int has_base;
    size_t current_change; size_t change_count; int linked_scrolling; int synchronized_selection;
    uint64_t revision;
} UmiVcsAdvancedCompareSession;
/**
 * Initialise vcs advanced compare session from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_compare_session_init(UmiVcsAdvancedCompareSession *session);
/**
 * Provide the vcs advanced compare session open operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_compare_session_open(UmiVcsAdvancedCompareSession *session,const char *session_id,
                                                 const UmiVcsAdvancedCompareSide *left,const UmiVcsAdvancedCompareSide *right,
                                                 const UmiVcsAdvancedCompareSide *base);
/**
 * Provide the vcs advanced compare session set change operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_compare_session_set_change(UmiVcsAdvancedCompareSession *session,size_t index,size_t count);
#ifdef __cplusplus
}
#endif
#endif
