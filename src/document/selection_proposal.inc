/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/selection_proposal.inc
 * PURPOSE: Compose selected edits over the existing replacement capture and document Undo transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/proposal.h"

/* Included after replacement_review.inc so proposals share its authoritative
 * stale-state checks and commit path instead of creating another text store. */
struct UmiDocumentProposal
{
    UmiDocumentReplacementPlan *target;
    UmiDocumentProposalSummary summary;
};
void UmiDocumentProposalDestroy(UmiDocumentProposal *proposal)
{
    if (proposal == NULL)
        return;
    UmiDocumentReplacementPlanDestroy(proposal->target);
    free(proposal);
}
UmiStatus UmiDocumentProposalCreate(UmiDocumentCoordinator *coordinator, UmiDocumentId document_id,
                                    UmiDocumentProposal **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (coordinator == NULL || document_id == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = EditDocumentIndex(coordinator, document_id);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiDocumentProposal *proposal = calloc(1U, sizeof(*proposal));
    if (proposal == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    proposal->target = calloc(1U, sizeof(*proposal->target));
    if (proposal->target == NULL)
    {
        free(proposal);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiDocumentReplacementPlan *target = proposal->target;
    UmiStatus status = UmiDocumentCoordinatorPrepareEdit(coordinator, document_id, &target->capture);
    if (status == UMI_STATUS_OK && target->capture->length > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && target->capture->view.read_only)
        status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK && target->capture->view.selection_length == 0U)
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK && !umi_document_utf8_validate((const unsigned char *)target->capture->text,
                                                               target->capture->length, NULL))
        status = UMI_STATUS_INVALID_STATE;
    UmiDocumentWorkingCopySnapshot current;
    if (status == UMI_STATUS_OK)
        status = snapshot_index(coordinator, index, &current);
    if (status != UMI_STATUS_OK)
    {
        UmiDocumentProposalDestroy(proposal);
        return status;
    }
    /* Capture save/conflict metadata once. Every subsequent proposal revision
     * continues to answer the same source question, never a silently newer one. */
    const UmiDocumentCoordinatorEntry *entry = &coordinator->entries[index];
    target->baseline = entry->baseline;
    target->encoding = entry->encoding;
    target->lineEnding = entry->line_ending;
    target->conflict = entry->conflict;
    target->summary.document_id = document_id;
    (void)snprintf(target->summary.display_name, sizeof(target->summary.display_name), "%s",
                   target->capture->stored.display_name);
    target->summary.previous_bytes = target->capture->length;
    target->summary.match_count = 1U;
    target->summary.has_unsaved_changes = current.dirty;
    proposal->summary.document_id = document_id;
    proposal->summary.revision = 1U;
    memcpy(proposal->summary.display_name, target->summary.display_name,
           sizeof(proposal->summary.display_name));
    proposal->summary.selection_offset = target->capture->view.cursor_offset;
    proposal->summary.selection_bytes = target->capture->view.selection_length;
    proposal->summary.previous_bytes = target->capture->length;
    proposal->summary.has_unsaved_changes = current.dirty;
    *out = proposal;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentProposalInspect(const UmiDocumentProposal *proposal, UmiDocumentProposalSummary *out)
{
    if (proposal == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = proposal->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentProposalSelection(const UmiDocumentProposal *proposal, const char **out, size_t *bytes)
{
    if (out != NULL)
        *out = NULL;
    if (bytes != NULL)
        *bytes = 0U;
    if (proposal == NULL || out == NULL || bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = proposal->target->capture->text + proposal->summary.selection_offset;
    *bytes = proposal->summary.selection_bytes;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentProposalSetText(UmiDocumentProposal *proposal, uint64_t expected_revision,
                                     const char *text, size_t byte_count)
{
    if (proposal == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (proposal->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    if (proposal->summary.revision != expected_revision)
        return UMI_STATUS_BUSY;
    if (proposal->summary.revision == UINT64_MAX || byte_count > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(text, '\0', byte_count) != NULL ||
        !umi_document_utf8_validate((const unsigned char *)text, byte_count, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDocumentReplacementPlan *target = proposal->target;
    size_t retained = target->capture->length - proposal->summary.selection_bytes;
    if (byte_count > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES - retained)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t total = retained + byte_count, offset = proposal->summary.selection_offset;
    char *proposed = malloc(total + 1U);
    if (proposed == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    /* Build a complete immutable comparison before replacing the old one.
     * This also permits callers to source text from the previous comparison. */
    memcpy(proposed, target->capture->text, offset);
    memcpy(proposed + offset, text, byte_count);
    size_t tail = offset + proposal->summary.selection_bytes;
    memcpy(proposed + offset + byte_count, target->capture->text + tail, target->capture->length - tail);
    proposed[total] = '\0';
    int changed = total != target->capture->length || memcmp(proposed, target->capture->text, total) != 0;
    free(target->proposed);
    target->proposed = proposed;
    target->summary.proposed_bytes = total;
    target->summary.text_changes = changed;
    proposal->summary.proposed_bytes = total;
    proposal->summary.text_changes = changed;
    proposal->summary.has_proposal = 1;
    ++proposal->summary.revision;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentProposalTexts(const UmiDocumentProposal *proposal, const char **previous,
                                   size_t *previous_bytes, const char **proposed, size_t *proposed_bytes)
{
    if (previous != NULL)
        *previous = NULL;
    if (previous_bytes != NULL)
        *previous_bytes = 0U;
    if (proposed != NULL)
        *proposed = NULL;
    if (proposed_bytes != NULL)
        *proposed_bytes = 0U;
    if (proposal == NULL || previous == NULL || previous_bytes == NULL || proposed == NULL ||
        proposed_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!proposal->summary.has_proposal || proposal->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    return UmiDocumentReplacementPlanTexts(proposal->target, previous, previous_bytes, proposed,
                                           proposed_bytes);
}
UmiStatus UmiDocumentProposalCheck(UmiDocumentCoordinator *coordinator, const UmiDocumentProposal *proposal)
{
    if (proposal == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiDocumentCoordinatorCheckReplacement(coordinator, proposal->target);
}
UmiStatus UmiDocumentProposalApply(UmiDocumentCoordinator *coordinator, UmiDocumentProposal *proposal,
                                   uint64_t reviewed_revision, int approved)
{
    if (coordinator == NULL || proposal == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (proposal->summary.applied || !proposal->summary.has_proposal)
        return UMI_STATUS_INVALID_STATE;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (reviewed_revision != proposal->summary.revision)
        return UMI_STATUS_BUSY;
    if (proposal->summary.revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiDocumentCoordinatorApplyReplacement(coordinator, proposal->target, NULL);
    if (status == UMI_STATUS_OK)
    {
        proposal->summary.applied = 1;
        ++proposal->summary.revision;
    }
    return status;
}
