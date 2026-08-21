/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/recovery_bridge.c
 *
 * PURPOSE:
 *   Build and apply recoverability decisions with visible revision and diff
 *   evidence rather than silently overwriting a live layout.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/recovery_bridge.h"
#include "internal.h"
#include <stdio.h>


void umi_workbench_designer_recovery_proposal_init(
    UmiWorkbenchDesignerRecoveryProposal *proposal)
{
    if (proposal == NULL) return;
    (void)memset(proposal, 0, sizeof(*proposal));
    proposal->recommended_choice =
        UMI_WORKBENCH_DESIGNER_RECOVERY_KEEP_CURRENT;
}

UmiStatus umi_workbench_designer_recovery_proposal_build(
    UmiWorkbenchDesignerRecoveryProposal *proposal,
    const UmiWorkbenchLayoutDocument *saved,
    const UmiWorkbenchLayoutDocument *current,
    const UmiWorkbenchLayoutDocument *recovered)
{
    UmiStatus status;
    int written;
    if (proposal == NULL || saved == NULL || current == NULL || recovered == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_workbench_designer_recovery_proposal_init(proposal);
    {
        UmiWorkbenchLayoutDiffOptions options =
            umi_workbench_layout_diff_options_default();
        status = umi_workbench_layout_compare(
            current, recovered, &options, &proposal->current_to_recovered);
    }
    if (status != UMI_STATUS_OK) return status;
    {
        UmiWorkbenchLayoutDiffOptions options =
            umi_workbench_layout_diff_options_default();
        status = umi_workbench_layout_compare(
            saved, recovered, &options, &proposal->saved_to_recovered);
    }
    if (status != UMI_STATUS_OK) return status;
    proposal->recovery_available = proposal->current_to_recovered.entry_count > 0U ||
        recovered->version.revision > current->version.revision;
    proposal->current_dirty =
        (current->flags & UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY) != 0U;
    proposal->recovered_newer =
        recovered->version.revision > current->version.revision;
    proposal->saved_newer = saved->version.revision > recovered->version.revision;
    if (proposal->recovery_available && proposal->recovered_newer &&
        !proposal->saved_newer) {
        proposal->recommended_choice =
            UMI_WORKBENCH_DESIGNER_RECOVERY_USE_RECOVERED;
    } else if (proposal->saved_newer) {
        proposal->recommended_choice =
            UMI_WORKBENCH_DESIGNER_RECOVERY_USE_SAVED;
    }
    written = snprintf(
        proposal->summary, sizeof(proposal->summary),
        "Recovery has %zu change(s) from the current layout and %zu change(s) from the saved layout.",
        proposal->current_to_recovered.entry_count,
        proposal->saved_to_recovered.entry_count);
    if (written < 0 || (size_t)written >= sizeof(proposal->summary)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    proposal->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_designer_recovery_apply(
    const UmiWorkbenchDesignerRecoveryProposal *proposal,
    UmiWorkbenchDesignerRecoveryChoice choice,
    const UmiWorkbenchLayoutDocument *saved,
    const UmiWorkbenchLayoutDocument *current,
    const UmiWorkbenchLayoutDocument *recovered,
    UmiWorkbenchLayoutDocument *out_document)
{
    const UmiWorkbenchLayoutDocument *selected;
    UmiStatus status;
    if (proposal == NULL || saved == NULL || current == NULL ||
        recovered == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (choice == UMI_WORKBENCH_DESIGNER_RECOVERY_USE_RECOVERED) {
        if (!proposal->recovery_available) return UMI_STATUS_INVALID_STATE;
        selected = recovered;
    } else if (choice == UMI_WORKBENCH_DESIGNER_RECOVERY_USE_SAVED) {
        selected = saved;
    } else if (choice == UMI_WORKBENCH_DESIGNER_RECOVERY_KEEP_CURRENT) {
        selected = current;
    } else {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_document_copy(out_document, selected);
    if (status != UMI_STATUS_OK) return status;
    if (choice == UMI_WORKBENCH_DESIGNER_RECOVERY_USE_RECOVERED) {
        out_document->flags |= UMI_WORKBENCH_LAYOUT_DOCUMENT_RECOVERED;
        out_document->flags |= UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY;
        umi_workbench_layout_document_increment_revision(out_document);
    }
    return UMI_STATUS_OK;
}
