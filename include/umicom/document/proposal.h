/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/proposal.h
 * PURPOSE: Review a proposed replacement for a captured source selection before changing its draft.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_PROPOSAL_H
#define UMICOM_DOCUMENT_PROPOSAL_H
#include "umicom/document/replacement.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiDocumentProposal UmiDocumentProposal;
    typedef struct UmiDocumentProposalSummary
    {
        UmiDocumentId document_id;
        char display_name[UMI_DOCUMENT_NAME_CAPACITY];
        uint64_t revision;
        size_t selection_offset, selection_bytes, previous_bytes, proposed_bytes;
        int has_proposal, text_changes, has_unsaved_changes, applied;
    } UmiDocumentProposalSummary;

    /* Capture the complete current draft and its nonempty selection. The same
 * coordinator/store/workbench must remain alive through Check and Apply;
 * inspection, editing proposed text and destruction use only owned storage.
 * Use the document owner's thread. Read-only or malformed UTF-8 drafts fail.
 * No provider, clipboard, file read/write or draft mutation occurs here.
 * *out is NULL on failure. Later caret/tab movement does not change the target. */
    UmiStatus UmiDocumentProposalCreate(UmiDocumentCoordinator *coordinator, UmiDocumentId document_id,
                                        UmiDocumentProposal **out);
    void UmiDocumentProposalDestroy(UmiDocumentProposal *proposal);
    /* Output is unchanged on error. A summary remains readable after Apply. */
    UmiStatus UmiDocumentProposalInspect(const UmiDocumentProposal *proposal,
                                         UmiDocumentProposalSummary *out);
    /* Borrow the selected bytes, not necessarily NUL-terminated, until Destroy.
 * Outputs are cleared on invalid arguments. The selection is a capture and
 * remains readable after Apply; it is never refreshed from a later draft. */
    UmiStatus UmiDocumentProposalSelection(const UmiDocumentProposal *proposal, const char **out,
                                           size_t *bytes);
    /* Replace proposed text only, using exactly byte_count readable bytes.
 * Empty text is an explicit deletion proposal. Reject NUL, invalid UTF-8,
 * stale proposal revision or excessive resulting document size atomically.
 * Success advances revision and invalidates any previous presentation/approval.
 * This does not interpret Markdown, patch syntax, commands or model instructions. */
    UmiStatus UmiDocumentProposalSetText(UmiDocumentProposal *proposal, uint64_t expected_revision,
                                         const char *text, size_t byte_count);
    /* Borrow complete before/after document text only after SetText and before
 * Apply. Outputs clear on error. Successful SetText, Apply or Destroy ends the
 * borrowed comparison lifetime. A native view must copy before notifying observers. */
    UmiStatus UmiDocumentProposalTexts(const UmiDocumentProposal *proposal, const char **previous,
                                       size_t *previous_bytes, const char **proposed, size_t *proposed_bytes);
    /* Check the captured identity, path, saved/draft revisions, external-change
 * state and exact bytes. Other tabs or caret movement alone do not stale the
 * captured selection. The target's text or save state changing requires a new
 * proposal. A stale proposal is never rebased or applied to another document. */
    UmiStatus UmiDocumentProposalCheck(UmiDocumentCoordinator *coordinator,
                                       const UmiDocumentProposal *proposal);
    /* Explicit approval and the revision presented to the user are both required.
 * Apply exactly the reviewed full draft through the existing document/Undo
 * transaction. One Undo restores the captured draft; earlier unsynchronized
 * typing keeps its own step. No saved file is written. Success consumes the
 * proposal, including unchanged text; failures leave it available for inspection. */
    UmiStatus UmiDocumentProposalApply(UmiDocumentCoordinator *coordinator, UmiDocumentProposal *proposal,
                                       uint64_t reviewed_revision, int approved);
#ifdef __cplusplus
}
#endif
#endif
