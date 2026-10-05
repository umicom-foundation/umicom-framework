/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/language_runtime/completion_preview.h
 * PURPOSE: Prepare complete completion previews without borrowing or changing a live document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LANGUAGE_RUNTIME_COMPLETION_PREVIEW_H
#define UMICOM_LANGUAGE_RUNTIME_COMPLETION_PREVIEW_H
#include "umicom/language_runtime/completion_catalogue.h"
#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct UmiLanguageCompletionPreview UmiLanguageCompletionPreview;
    /* Own the complete result of one choice against copied source. This is useful
 * for worker tasks: no document coordinator or GTK buffer is accessed.
 * source_bytes excludes NUL. Embedded NUL and invalid UTF-8 are refused.
 * fallback_begin/end and cursor_offset are UTF-8 byte offsets. Supply the
 * editor's word/selection range, or an empty range at the caret. They must
 * describe one line containing the caret; server ranges take precedence.
 * Maximum source and resulting text: 16 MiB. The completion planner's edit
 * limits still apply. Borrow all inputs until return, then destroy them freely.
 * Cancellation or failure clears out_preview without publishing partial text.
 * A preview grants no permission to edit the live document: stage it through
 * a captured document source request and obtain explicit acceptance. */
    UmiStatus UmiLanguageCompletionPreviewCreate(
        const UmiLanguageCompletionCatalogue *catalogue, size_t choice, const char *document_uri,
        const char *provider_id, const char *source, size_t source_bytes, size_t cursor_offset,
        size_t fallback_begin, size_t fallback_end, UmiLanguageCompletionAcceptance acceptance,
        const UmiCancellationToken *cancel, UmiLanguageCompletionPreview **out_preview);
    void UmiLanguageCompletionPreviewDestroy(UmiLanguageCompletionPreview *preview);
    /* Borrow immutable, NUL-terminated full text until Destroy. The returned byte
 * count excludes NUL. The caret follows the primary replacement, adjusted for
 * additional edits before it; additional edits after it do not move the caret.
 * No selection or snippet placeholders are created. Outputs clear on error. */
    UmiStatus UmiLanguageCompletionPreviewRead(const UmiLanguageCompletionPreview *preview,
                                               const char **out_text, size_t *out_bytes, size_t *out_cursor);
    size_t UmiLanguageCompletionPreviewEditCount(const UmiLanguageCompletionPreview *preview);
#ifdef __cplusplus
}
#endif
#endif
