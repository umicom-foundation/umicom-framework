/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/text_edit_preview.h
 * PURPOSE: Construct an owned document preview from a language server text-edit response.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_TEXT_EDIT_PREVIEW_H
#define UMICOM_LANGUAGE_RUNTIME_TEXT_EDIT_PREVIEW_H
#include "umicom/editor/text_position.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageTextEditPreview UmiLanguageTextEditPreview;
    /* Interpret a TextEdit array or null against the exact supplied UTF-8 source.
 * Positions use zero-based UTF-16 units. Support at most 256 edits and a 1 MiB
 * result JSON document; source and proposed text are each limited to 16 MiB.
 * All ranges refer to the original source. Sort by source position while
 * preserving response order for inserts at the same position; such inserts
 * may precede one replacement there. Refuse overlapping ranges, annotations,
 * malformed text and coordinates inside a surrogate pair or CRLF.
 *
 * No live document or file is changed. The preview owns the complete result.
 * A caret inside replaced text moves to the replacement end. A caret at an
 * insertion moves after inserted text. Other carets retain their source-relative
 * position. Refuse a mapped caret that would split a new CRLF boundary.
 * Failure clears out_preview. Inputs are borrowed only until return. */
    UmiStatus UmiLanguageTextEditPreviewCreate(const void *result_json, size_t result_bytes,
                                               const char *source, size_t source_bytes, size_t caret,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageTextEditPreview **out_preview);
    /* Also validate JSON-RPC correlation and errors. This does not launch a server.
 * The same response size limit includes the protocol envelope. */
    UmiStatus UmiLanguageTextEditPreviewReadResponse(const void *response_json, size_t response_bytes,
                                                     uint64_t expected_request_id, const char *source,
                                                     size_t source_bytes, size_t caret,
                                                     const UmiCancellationToken *cancel,
                                                     UmiLanguageTextEditPreview **out_preview);
    void UmiLanguageTextEditPreviewDestroy(UmiLanguageTextEditPreview *preview);
    /* Borrow complete result bytes until Destroy; outputs are unchanged on error. */
    UmiStatus UmiLanguageTextEditPreviewRead(const UmiLanguageTextEditPreview *preview, const char **out_text,
                                             size_t *out_bytes, size_t *out_caret);
    size_t UmiLanguageTextEditPreviewCount(const UmiLanguageTextEditPreview *preview);
#ifdef __cplusplus
}
#endif
#endif
