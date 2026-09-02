/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/compare_session.c
 *
 * PURPOSE:
 *   Implement professional comparison session identity and synchronized state.
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

#include "umicom/vcs/advanced/compare_session.h"

#include <string.h>

/*
 * Initialise vcs advanced compare session from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_compare_session_init(UmiVcsAdvancedCompareSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) {
        return;
    }
    (void)memset(session, 0, sizeof(*session));
    session->struct_size = (uint32_t)sizeof(*session);
    session->api_version = UMI_VCS_ADVANCED_API_VERSION;
    session->linked_scrolling = 1;
    session->synchronized_selection = 1;
    session->revision = 1U;
}

/*
 * Provide the vcs advanced compare session open operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_compare_session_open(
    UmiVcsAdvancedCompareSession *session,
    const char *session_id,
    const UmiVcsAdvancedCompareSide *left,
    const UmiVcsAdvancedCompareSide *right,
    const UmiVcsAdvancedCompareSide *base)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL ||
        umi_vcs_advanced_compare_side_validate(left) != UMI_STATUS_OK ||
        umi_vcs_advanced_compare_side_validate(right) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_vcs_advanced_copy_text(
        session->session_id, sizeof(session->session_id), session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    session->left = *left;
    session->right = *right;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (base != NULL) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (umi_vcs_advanced_compare_side_validate(base) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        session->base = *base;
        session->has_base = 1;
        session->mode = UMI_VCS_COMPARE_THREE_WAY;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        session->mode = UMI_VCS_COMPARE_TWO_WAY;
    }
    session->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced compare session set change operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_compare_session_set_change(
    UmiVcsAdvancedCompareSession *session,
    size_t index,
    size_t count)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || (count > 0U && index >= count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->current_change = index;
    session->change_count = count;
    session->revision += 1U;
    return UMI_STATUS_OK;
}
