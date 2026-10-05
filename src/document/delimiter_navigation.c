/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/delimiter_navigation.c
 * PURPOSE: Compose immutable source capture, delimiter discovery and existing document navigation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/delimiter_navigation.h"
#include "umicom/document/source_request.h"
#include <string.h>
UmiStatus UmiDocumentCoordinatorNavigateDelimiter(UmiDocumentCoordinator *coordinator, UmiDocumentId document,
                                                  UmiDocumentDelimiterAction action,
                                                  UmiEditorDelimiterSyntax syntax)
{
    if (action != UMI_DOCUMENT_DELIMITER_MATCH && action != UMI_DOCUMENT_DELIMITER_CONTENTS &&
        action != UMI_DOCUMENT_DELIMITER_PAIR)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDocumentSourceRequest *request = NULL;
    UmiDocumentSourceRequestSummary summary;
    UmiStatus status = UmiDocumentSourceRequestCreateInspection(coordinator, document, &request);
    const char *text = NULL;
    size_t bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestInspect(request, &summary);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestRead(request, &text, &bytes);
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = text;
    view.byte_count = bytes;
    view.capacity = bytes;
    UmiEditorDelimiterPair pair;
    size_t start = 0U, end = 0U;
    if (status == UMI_STATUS_OK)
        status = action == UMI_DOCUMENT_DELIMITER_MATCH
                     ? UmiEditorDelimiterMatch(&view, summary.cursor_offset, syntax, &pair)
                     : UmiEditorDelimiterEnclosing(&view, summary.cursor_offset, syntax, &pair);
    if (status == UMI_STATUS_OK)
    {
        if (action == UMI_DOCUMENT_DELIMITER_MATCH)
            start = end =
                pair.anchor_offset == pair.opening_offset ? pair.closing_offset : pair.opening_offset;
        else
        {
            start = pair.opening_offset + (action == UMI_DOCUMENT_DELIMITER_CONTENTS ? 1U : 0U);
            end = pair.closing_offset + (action == UMI_DOCUMENT_DELIMITER_PAIR ? 1U : 0U);
        }
    }
    UmiEditorTextPosition first, last;
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, start, &first);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, end, &last);
    /* Discovery never edits or synchronizes the draft into history. Recheck
     * the capture, then let the coordinator own tab activation and selection. */
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestCheck(coordinator, request);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentCoordinatorSelectSourceRange(coordinator, document, first, last, NULL, NULL);
    UmiDocumentSourceRequestDestroy(request);
    return status;
}

UmiStatus UmiDocumentCoordinatorNavigateActiveDelimiter(UmiDocumentCoordinator *coordinator,
                                                        UmiDocumentDelimiterAction action)
{
    if (action != UMI_DOCUMENT_DELIMITER_MATCH && action != UMI_DOCUMENT_DELIMITER_CONTENTS &&
        action != UMI_DOCUMENT_DELIMITER_PAIR)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDocumentWorkingCopySnapshot active;
    UmiStatus status = umi_document_coordinator_active_snapshot(coordinator, &active);
    if (status != UMI_STATUS_OK)
        return status;
    /* Match explicit language identities. A C++ raw string, for example,
     * cannot safely inherit C string rules merely because its suffix is c. */
    UmiEditorDelimiterSyntax syntax = UMI_EDITOR_DELIMITER_LITERAL;
    if (strcmp(active.language_id, "c") == 0)
        syntax = UMI_EDITOR_DELIMITER_C;
    else if (strcmp(active.language_id, "json") == 0)
        syntax = UMI_EDITOR_DELIMITER_JSON;
    return UmiDocumentCoordinatorNavigateDelimiter(coordinator, active.document_id, action, syntax);
}
