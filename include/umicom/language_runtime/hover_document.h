/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/hover_document.h
 * PURPOSE: Own typed language hover content without interpreting executable markup.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_HOVER_DOCUMENT_H
#define UMICOM_LANGUAGE_RUNTIME_HOVER_DOCUMENT_H
#include "umicom/editor/text_position.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum UmiLanguageHoverContentKind
    {
        UMI_LANGUAGE_HOVER_PLAIN_TEXT,
        UMI_LANGUAGE_HOVER_MARKDOWN,
        UMI_LANGUAGE_HOVER_CODE
    } UmiLanguageHoverContentKind;
    typedef struct UmiLanguageHoverBlock
    {
        UmiLanguageHoverContentKind kind;
        const char *language, *text;
        size_t bytes;
    } UmiLanguageHoverBlock;
    typedef struct UmiLanguageHoverDocument UmiLanguageHoverDocument;
    /* Decode a Hover object or null from a bounded, exact JSON span. Support
 * MarkupContent, a legacy markdown string, and legacy string/code-block arrays.
 * Require valid UTF-8, known markup kinds and correctly shaped optional ranges.
 * Limit the JSON to 1 MiB, 256 blocks and 127 UTF-8 bytes per code language.
 * No source, file, URL, HTML or code is evaluated. Failure clears out_document.
 * All returned strings are owned by the document until Destroy. */
    UmiStatus UmiLanguageHoverDocumentCreate(const void *result_json, size_t bytes,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageHoverDocument **out_document);
    /* Correlate a positive numeric JSON-RPC ID and validate errors before decoding.
 * The same byte limit includes the envelope. */
    UmiStatus UmiLanguageHoverDocumentReadResponse(const void *response_json, size_t bytes,
                                                   uint64_t expected_request_id,
                                                   const UmiCancellationToken *cancel,
                                                   UmiLanguageHoverDocument **out_document);
    void UmiLanguageHoverDocumentDestroy(UmiLanguageHoverDocument *document);
    size_t UmiLanguageHoverDocumentCount(const UmiLanguageHoverDocument *document);
    UmiStatus UmiLanguageHoverDocumentAt(const UmiLanguageHoverDocument *document, size_t index,
                                         UmiLanguageHoverBlock *out_block);
    /* Join block values with a blank line for literal text display. Markdown is
 * retained as text, not rendered. A host adding rich rendering must sanitize
 * it and require a separate explicit action before opening external links. */
    UmiStatus UmiLanguageHoverDocumentText(const UmiLanguageHoverDocument *document, const char **out_text,
                                           size_t *out_bytes);
    /* NOT_FOUND means no range, including a null result. Coordinates are UTF-16
 * positions but are not checked against source here. Resolve them against the
 * matching captured source before highlighting. Errors preserve outputs. */
    UmiStatus UmiLanguageHoverDocumentRange(const UmiLanguageHoverDocument *document,
                                            UmiEditorTextPosition *out_start, UmiEditorTextPosition *out_end);
#ifdef __cplusplus
}
#endif
#endif
