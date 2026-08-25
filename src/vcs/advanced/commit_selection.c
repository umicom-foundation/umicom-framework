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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/commit_selection.h"

#include <string.h>

void umi_vcs_advanced_commit_selection_init(
    UmiVcsAdvancedCommitSelection *selection)
{
    if (selection == NULL) {
        return;
    }
    (void)memset(selection, 0, sizeof(*selection));
    selection->struct_size = (uint32_t)sizeof(*selection);
    selection->api_version = UMI_VCS_ADVANCED_API_VERSION;
}

int umi_vcs_advanced_commit_selection_contains(
    const UmiVcsAdvancedCommitSelection *selection,
    const char *oid)
{
    size_t i;
    if (selection == NULL || oid == NULL) {
        return 0;
    }
    for (i = 0U; i < selection->count; ++i) {
        if (umi_vcs_advanced_text_equal(selection->oids[i], oid)) {
            return 1;
        }
    }
    return 0;
}

UmiStatus umi_vcs_advanced_commit_selection_add(
    UmiVcsAdvancedCommitSelection *selection,
    const char *oid)
{
    UmiStatus status;
    if (selection == NULL || !umi_vcs_advanced_text_present(oid)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_vcs_advanced_commit_selection_contains(selection, oid)) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (selection->count >= UMI_VCS_ADVANCED_SMALL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_vcs_advanced_copy_text(
        selection->oids[selection->count],
        sizeof(selection->oids[0]),
        oid);
    if (status == UMI_STATUS_OK) {
        selection->count += 1U;
    }
    return status;
}

UmiStatus umi_vcs_advanced_commit_selection_remove(
    UmiVcsAdvancedCommitSelection *selection,
    const char *oid)
{
    size_t i;
    if (selection == NULL || oid == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (i = 0U; i < selection->count; ++i) {
        if (umi_vcs_advanced_text_equal(selection->oids[i], oid)) {
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
