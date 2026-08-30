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

void umi_vcs_advanced_compare_session_init(UmiVcsAdvancedCompareSession *session)
{
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

UmiStatus umi_vcs_advanced_compare_session_open(
    UmiVcsAdvancedCompareSession *session,
    const char *session_id,
    const UmiVcsAdvancedCompareSide *left,
    const UmiVcsAdvancedCompareSide *right,
    const UmiVcsAdvancedCompareSide *base)
{
    UmiStatus status;

    if (session == NULL ||
        umi_vcs_advanced_compare_side_validate(left) != UMI_STATUS_OK ||
        umi_vcs_advanced_compare_side_validate(right) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_vcs_advanced_copy_text(
        session->session_id, sizeof(session->session_id), session_id);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    session->left = *left;
    session->right = *right;
    if (base != NULL) {
        if (umi_vcs_advanced_compare_side_validate(base) != UMI_STATUS_OK) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        session->base = *base;
        session->has_base = 1;
        session->mode = UMI_VCS_COMPARE_THREE_WAY;
    } else {
        session->mode = UMI_VCS_COMPARE_TWO_WAY;
    }
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_compare_session_set_change(
    UmiVcsAdvancedCompareSession *session,
    size_t index,
    size_t count)
{
    if (session == NULL || (count > 0U && index >= count)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session->current_change = index;
    session->change_count = count;
    session->revision += 1U;
    return UMI_STATUS_OK;
}
