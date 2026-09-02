/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/commit_selection.c
 *
 * PURPOSE:
 *   Implement ordered de-duplicated commit selections.
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

#include "umicom/vcs/advanced/commit_selection.h"

#include <string.h>

/*
 * Initialise vcs advanced commit selection from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_commit_selection_init(
    UmiVcsAdvancedCommitSelection *selection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL) {
        return;
    }
    (void)memset(selection, 0, sizeof(*selection));
    selection->struct_size = (uint32_t)sizeof(*selection);
    selection->api_version = UMI_VCS_ADVANCED_API_VERSION;
}

/*
 * Provide the vcs advanced commit selection contains operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_commit_selection_contains(
    const UmiVcsAdvancedCommitSelection *selection,
    const char *oid)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || oid == NULL) {
        return 0;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < selection->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_vcs_advanced_text_equal(selection->oids[i], oid)) {
            return 1;
        }
    }
    return 0;
}

/*
 * Add vcs advanced commit selection only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_commit_selection_add(
    UmiVcsAdvancedCommitSelection *selection,
    const char *oid)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || !umi_vcs_advanced_text_present(oid)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_commit_selection_contains(selection, oid)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (selection->count >= UMI_VCS_ADVANCED_SMALL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_vcs_advanced_copy_text(
        selection->oids[selection->count],
        sizeof(selection->oids[0]),
        oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        selection->count += 1U;
    }
    return status;
}

/*
 * Remove vcs advanced commit selection while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_vcs_advanced_commit_selection_remove(
    UmiVcsAdvancedCommitSelection *selection,
    const char *oid)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || oid == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < selection->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_vcs_advanced_text_equal(selection->oids[i], oid)) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (i + 1U < selection->count) {
                (void)memmove(
                    selection->oids[i],
                    selection->oids[i + 1U],
                    (selection->count - i - 1U) *
                        sizeof(selection->oids[0]));
            }
            selection->count -= 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
