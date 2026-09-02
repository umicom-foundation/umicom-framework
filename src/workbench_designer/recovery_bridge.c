/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/recovery_bridge.c
 *
 * PURPOSE:
 *   Build and apply recoverability decisions with visible revision and diff
 *   evidence rather than silently overwriting a live layout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/recovery_bridge.h"
#include "internal.h"
#include <stdio.h>


/*
 * Initialise workbench designer recovery proposal from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_recovery_proposal_init(
    UmiWorkbenchDesignerRecoveryProposal *proposal)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (proposal == NULL) return;
    (void)memset(proposal, 0, sizeof(*proposal));
    proposal->recommended_choice =
        UMI_WORKBENCH_DESIGNER_RECOVERY_KEEP_CURRENT;
}

/*
 * Provide the workbench designer recovery proposal build operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_recovery_proposal_build(
    UmiWorkbenchDesignerRecoveryProposal *proposal,
    const UmiWorkbenchLayoutDocument *saved,
    const UmiWorkbenchLayoutDocument *current,
    const UmiWorkbenchLayoutDocument *recovered)
{
    UmiStatus status;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    {
        UmiWorkbenchLayoutDiffOptions options =
            umi_workbench_layout_diff_options_default();
        status = umi_workbench_layout_compare(
            saved, recovered, &options, &proposal->saved_to_recovered);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    proposal->recovery_available = proposal->current_to_recovered.entry_count > 0U ||
        recovered->version.revision > current->version.revision;
    proposal->current_dirty =
        (current->flags & UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY) != 0U;
    proposal->recovered_newer =
        recovered->version.revision > current->version.revision;
    proposal->saved_newer = saved->version.revision > recovered->version.revision;
    /* Apply this branch only when its contract condition is satisfied. */
    if (proposal->recovery_available && proposal->recovered_newer &&
        !proposal->saved_newer) {
        proposal->recommended_choice =
            UMI_WORKBENCH_DESIGNER_RECOVERY_USE_RECOVERED;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (proposal->saved_newer) {
        proposal->recommended_choice =
            UMI_WORKBENCH_DESIGNER_RECOVERY_USE_SAVED;
    }
    written = snprintf(
        proposal->summary, sizeof(proposal->summary),
        "Recovery has %zu change(s) from the current layout and %zu change(s) from the saved layout.",
        proposal->current_to_recovered.entry_count,
        proposal->saved_to_recovered.entry_count);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(proposal->summary)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    proposal->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Perform workbench designer recovery through the module contract so client applications
 * do not duplicate its policy.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (proposal == NULL || saved == NULL || current == NULL ||
        recovered == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (choice == UMI_WORKBENCH_DESIGNER_RECOVERY_USE_RECOVERED) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!proposal->recovery_available) return UMI_STATUS_INVALID_STATE;
        selected = recovered;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (choice == UMI_WORKBENCH_DESIGNER_RECOVERY_USE_SAVED) {
        selected = saved;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (choice == UMI_WORKBENCH_DESIGNER_RECOVERY_KEEP_CURRENT) {
        selected = current;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_layout_document_copy(out_document, selected);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (choice == UMI_WORKBENCH_DESIGNER_RECOVERY_USE_RECOVERED) {
        out_document->flags |= UMI_WORKBENCH_LAYOUT_DOCUMENT_RECOVERED;
        out_document->flags |= UMI_WORKBENCH_LAYOUT_DOCUMENT_DIRTY;
        umi_workbench_layout_document_increment_revision(out_document);
    }
    return UMI_STATUS_OK;
}
