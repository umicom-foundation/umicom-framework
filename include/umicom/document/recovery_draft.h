/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/recovery_draft.h
 * PURPOSE: Own complete recovery drafts independently of a live editor or a saved source file.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_RECOVERY_DRAFT_H
#define UMICOM_DOCUMENT_RECOVERY_DRAFT_H
#include "umicom/document/coordinator.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_DOCUMENT_RECOVERY_KEY_CAPACITY 33U
#define UMI_DOCUMENT_RECOVERY_TEXT_LIMIT (8U * 1024U * 1024U)
#define UMI_DOCUMENT_RECOVERY_RECORD_LIMIT                                                                   \
    (UMI_DOCUMENT_RECOVERY_TEXT_LIMIT + UMI_PATH_CAPACITY + UMI_DOCUMENT_NAME_CAPACITY +                     \
     UMI_DOCUMENT_LANGUAGE_CAPACITY + 512U)
    typedef struct UmiDocumentRecoveryInfo
    {
        char key[UMI_DOCUMENT_RECOVERY_KEY_CAPACITY];
        char display_name[UMI_DOCUMENT_NAME_CAPACITY];
        char source_path[UMI_PATH_CAPACITY];
        char language_id[UMI_DOCUMENT_LANGUAGE_CAPACITY];
        uint64_t source_revision;
        size_t text_bytes, cursor_offset, selection_bytes;
    } UmiDocumentRecoveryInfo;
    typedef struct UmiDocumentRecoveryDraft UmiDocumentRecoveryDraft;
    /* Generate a lowercase 128-bit hexadecimal identity using the system random
 * provider. A storage writer must still refuse an existing destination: random
 * identities reduce collisions but do not authorize overwriting another draft.
 * Failure clears nonempty output. No filesystem or network access occurs. */
    UmiStatus UmiDocumentRecoveryKeyCreate(char *out_key, size_t capacity);
    /* Copy complete UTF-8 source and metadata. The key must contain exactly 32
 * lowercase hexadecimal characters. Names are nonempty; path and language may
 * be empty. Metadata cannot contain ASCII control bytes. Source may contain
 * newlines, including bare CR, but no embedded NUL. Endpoints cannot split a
 * UTF-8 scalar or CRLF pair. The path is descriptive only and never opened.
 * Failure clears out_draft. Inputs are borrowed only until return. */
    UmiStatus UmiDocumentRecoveryDraftCreate(const UmiDocumentRecoveryInfo *info, const char *text,
                                             size_t bytes, const UmiCancellationToken *cancel,
                                             UmiDocumentRecoveryDraft **out_draft);
    void UmiDocumentRecoveryDraftDestroy(UmiDocumentRecoveryDraft *draft);
    UmiStatus UmiDocumentRecoveryDraftInspect(const UmiDocumentRecoveryDraft *draft,
                                              UmiDocumentRecoveryInfo *out_info);
    /* Borrow source until Destroy. Failure clears both outputs. */
    UmiStatus UmiDocumentRecoveryDraftRead(const UmiDocumentRecoveryDraft *draft, const char **out_text,
                                           size_t *out_bytes);
    /* Encode a complete bounded record; callers free output with BytesFree. Text
 * is length-framed rather than escaped, so large source does not expand into
 * a much larger JSON string. Decode validates every field before publishing
 * an owner. Neither function changes a live document, file or Undo history.
 * Cancellation is checked between bounded validation/allocation phases. */
    UmiStatus UmiDocumentRecoveryDraftEncode(const UmiDocumentRecoveryDraft *draft, unsigned char **out_bytes,
                                             size_t *out_size);
    UmiStatus UmiDocumentRecoveryDraftDecode(const void *bytes, size_t size,
                                             const UmiCancellationToken *cancel,
                                             UmiDocumentRecoveryDraft **out_draft);
    void UmiDocumentRecoveryBytesFree(void *bytes);
#ifdef __cplusplus
}
#endif
#endif
