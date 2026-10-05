/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/source_request.h
 * PURPOSE: Capture a source request and review its complete draft result through the document owner.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_SOURCE_REQUEST_H
#define UMICOM_DOCUMENT_SOURCE_REQUEST_H
#include "umicom/document/replacement.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiDocumentSourceRequest UmiDocumentSourceRequest;
    typedef struct UmiDocumentSourceRequestSummary
    {
        UmiDocumentId document_id;
        char display_name[UMI_DOCUMENT_NAME_CAPACITY];
        char uri[UMI_DOCUMENT_URI_CAPACITY];
        char language_id[UMI_DOCUMENT_LANGUAGE_CAPACITY];
        size_t source_bytes, cursor_offset, selection_bytes;
        size_t proposed_bytes, proposed_cursor;
        uint64_t revision;
        int has_proposal, text_changes, applied;
    } UmiDocumentSourceRequestSummary;
    /* Capture complete UTF-8 draft bytes, document identity and caret/selection.
 * This is intended for completion, formatting and other source tools whose
 * response may arrive later. It reads no saved file and changes no history.
 * Empty drafts and empty selections are valid. Read-only drafts are refused.
 *
 * Use the coordinator's owning thread. Keep that same coordinator, store and
 * workbench alive through Check/Apply. Copy captured bytes/metadata before
 * giving work to another thread; do not share a mutable request across threads.
 * Failure clears out_request. The caller destroys every successful request. */
    UmiStatus UmiDocumentSourceRequestCreate(UmiDocumentCoordinator *coordinator, UmiDocumentId document_id,
                                             UmiDocumentSourceRequest **out_request);
    /* Capture for read-only tools such as source information. Both writable
     * and read-only drafts are accepted. Stage and Apply always return
     * PERMISSION_DENIED for this request, even if the document later becomes
     * writable. Check still rejects permission, identity, content and caret
     * changes. Ownership and thread rules are the same as Create above. */
    UmiStatus UmiDocumentSourceRequestCreateInspection(UmiDocumentCoordinator *coordinator,
                                                       UmiDocumentId document_id,
                                                       UmiDocumentSourceRequest **out_request);
    void UmiDocumentSourceRequestDestroy(UmiDocumentSourceRequest *request);
    /* Inspect copies of metadata and borrow complete captured source until Destroy.
 * Source is immutable and remains readable after Apply. Output metadata is
 * unchanged on error; source output pointers/lengths are cleared on error. */
    UmiStatus UmiDocumentSourceRequestInspect(const UmiDocumentSourceRequest *request,
                                              UmiDocumentSourceRequestSummary *out_summary);
    UmiStatus UmiDocumentSourceRequestRead(const UmiDocumentSourceRequest *request, const char **out_text,
                                           size_t *out_bytes);
    /* Stage a complete UTF-8 result, not an insertion or a patch. No live document
 * changes. Reject embedded NUL, invalid UTF-8, excess size, invalid cursor
 * boundaries and an old review revision. Empty text explicitly deletes the
 * draft. Success advances the review revision and invalidates old approval.
 * The proposed cursor is a UTF-8 byte offset, not a display-column count. */
    UmiStatus UmiDocumentSourceRequestStage(UmiDocumentSourceRequest *request, uint64_t expected_revision,
                                            const char *text, size_t bytes, size_t cursor_offset);
    /* Borrow the exact proposed text after Stage and before Apply. Its lifetime
 * ends on the next successful Stage, Apply or Destroy. Outputs clear on error. */
    UmiStatus UmiDocumentSourceRequestProposed(const UmiDocumentSourceRequest *request, const char **out_text,
                                               size_t *out_bytes);
    /* Recheck identity, draft/save revisions, path, conflict markers, full bytes
 * and captured caret/selection. A changed active tab does not redirect the
 * request. The current caret and selection must match the capture. Hosts should
 * also invalidate a request on navigation if moving away and back must count. */
    UmiStatus UmiDocumentSourceRequestCheck(UmiDocumentCoordinator *coordinator,
                                            const UmiDocumentSourceRequest *request);
    /* Apply only the revision explicitly approved by the caller. One Undo restores
 * the captured draft; earlier unsynchronized typing keeps its own Undo step.
 * Keep the current active tab. No provider or filesystem write occurs.
 * Failure preserves the request for inspection. Success consumes approval,
 * including an unchanged result; the caller still destroys the request. */
    UmiStatus UmiDocumentSourceRequestApply(UmiDocumentCoordinator *coordinator,
                                            UmiDocumentSourceRequest *request, uint64_t reviewed_revision,
                                            int approved);
#ifdef __cplusplus
}
#endif
#endif
