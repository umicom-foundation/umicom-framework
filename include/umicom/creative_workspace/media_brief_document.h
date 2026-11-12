/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/media_brief_document.h
 *
 * PURPOSE:
 *   Preserve editable creation inputs and reviewed text without restoring generation permission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_CREATIVE_MEDIA_BRIEF_DOCUMENT_H
#define UMICOM_CREATIVE_MEDIA_BRIEF_DOCUMENT_H
#include "umicom/creative_workspace/media_brief.h"
#include "umicom/creative_workspace/asset.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_MEDIA_BRIEF_DOCUMENT_BYTES (64U * 1024U)
typedef struct UmiMediaBriefDocument {
    UmiMediaBriefDraft draft;
    char prompt[UMI_MEDIA_BRIEF_PROMPT_CAPACITY];
} UmiMediaBriefDocument;
/* Recovery accepts empty or unfinished title/source/prompt fields. Every array
 * must be terminated and contain valid UTF-8 with only tab/CR/LF ASCII controls.
 * Kind remains one of the four supported creation tasks. Preparation still
 * requires a visible title and source through UmiMediaBriefCreate.
 * Prompt is independently edited text: it need not match the current source.
 * A recovered document grants no readiness, model selection or send approval. */
UmiStatus UmiMediaBriefDocumentValidate(const UmiMediaBriefDocument *document);
/* Export produces an immutable document asset. Initialise *out=NULL; failure
 * preserves it. Import admits the complete version-1 JSON shape into a private
 * candidate before replacing out, which remains unchanged on every failure.
 * Inputs stay readable and stable for the call. No file/network access occurs.
 * Credentials, receipts, model settings and working paths are not serialized. */
UmiStatus UmiMediaBriefDocumentExport(const UmiMediaBriefDocument *document,
    const UmiCancellationToken *cancel, UmiCreativeAsset **out);
UmiStatus UmiMediaBriefDocumentImport(const void *bytes, size_t size,
    const UmiCancellationToken *cancel, UmiMediaBriefDocument *out);
/* Explicit ordinary absolute paths in trusted parent directories only.
 * Save uses the shared atomic writer and may replace an existing file/create
 * parents; the caller obtains replacement approval. Concurrent writers are
 * last-completion-wins, not a compare-and-swap protocol. Once publication
 * begins, its actual outcome is returned even if cancellation arrives later.
 * Load uses the bounded regular-file reader and preserves out on failure.
 * Keep blocking file work away from a UI thread. No automatic path is chosen. */
UmiStatus UmiMediaBriefDocumentSave(const UmiMediaBriefDocument *document,
    const char *absolute_path, const UmiCancellationToken *cancel);
UmiStatus UmiMediaBriefDocumentLoad(const char *absolute_path,
    const UmiCancellationToken *cancel, UmiMediaBriefDocument *out);
#ifdef __cplusplus
}
#endif
#endif
