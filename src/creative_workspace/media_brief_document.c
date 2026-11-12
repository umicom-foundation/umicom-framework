/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/media_brief_document.c
 *
 * PURPOSE:
 *   Encode and admit complete creation-draft recovery documents through shared JSON services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/creative_workspace/media_brief_document.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/security/secrets.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

/* Wire names are independent of enum ordinals and user-interface translations.
 * Unknown kinds are refused rather than mapped to a different creation task. */
static const char *const kind_names[] = {"storyboard", "image", "video", "music"};

UmiStatus UmiMediaBriefDocumentExport(const UmiMediaBriefDocument *document,
    const UmiCancellationToken *cancel, UmiCreativeAsset **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiMediaBriefDocumentValidate(document);
    if (status != UMI_STATUS_OK) return status;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    UmiCreativeWriter writer;
    UmiCreativeExport encoded = {0};
    UmiCreativeWriterInit(&writer, UMI_MEDIA_BRIEF_DOCUMENT_BYTES);
    UmiCreativeWriterText(&writer,
        "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":");
    UmiCreativeWriterJson(&writer, kind_names[document->draft.kind]);
    UmiCreativeWriterText(&writer, ",\"title\":");
    UmiCreativeWriterJson(&writer, document->draft.title);
    UmiCreativeWriterText(&writer, ",\"source\":");
    UmiCreativeWriterJson(&writer, document->draft.source);
    UmiCreativeWriterText(&writer, ",\"direction\":");
    UmiCreativeWriterJson(&writer, document->draft.direction);
    UmiCreativeWriterText(&writer, "},\"prompt\":");
    UmiCreativeWriterJson(&writer, document->prompt);
    UmiCreativeWriterText(&writer, "}\n");
    status = UmiCreativeWriterFinish(&writer, &encoded);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetCapture("Umicom Media creation draft",
            UMI_CREATIVE_ASSET_DOCUMENT, encoded.bytes, encoded.size, cancel, out);
    if (encoded.bytes != NULL) umi_secret_clear(encoded.bytes, encoded.size);
    UmiCreativeExportFree(&encoded);
    return status;
}
static UmiStatus Text(const UmiJsonTree *tree, int object, const char *key,
    char *out, size_t capacity)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeText(tree, node, out, capacity) : status;
}
static UmiStatus ReadDraft(const UmiJsonTree *tree, int node, UmiMediaBriefDraft *draft)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, node) != 4U) return UMI_STATUS_PARSE_ERROR;
    char kind[16] = {0};
    UmiStatus status = Text(tree, node, "kind", kind, sizeof(kind));
    if (status == UMI_STATUS_OK) {
        size_t at = 0U;
        while (at < sizeof(kind_names) / sizeof(kind_names[0]) &&
               strcmp(kind, kind_names[at]) != 0) ++at;
        if (at == sizeof(kind_names) / sizeof(kind_names[0])) status = UMI_STATUS_PARSE_ERROR;
        else draft->kind = (UmiMediaBriefKind)at;
    }
    if (status == UMI_STATUS_OK)
        status = Text(tree, node, "title", draft->title, sizeof(draft->title));
    if (status == UMI_STATUS_OK)
        status = Text(tree, node, "source", draft->source, sizeof(draft->source));
    if (status == UMI_STATUS_OK)
        status = Text(tree, node, "direction", draft->direction, sizeof(draft->direction));
    return status;
}
UmiStatus UmiMediaBriefDocumentImport(const void *bytes, size_t size,
    const UmiCancellationToken *cancel, UmiMediaBriefDocument *out)
{
    if (bytes == NULL || size == 0U || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_MEDIA_BRIEF_DOCUMENT_BYTES, 64U, 4U};
    UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiMediaBriefDocument *candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) {
        UmiJsonTreeDestroy(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    char format[40] = {0};
    int version_node = -1, draft_node = -1;
    int64_t version = 0;
    /* Exact shape admission rejects future fields instead of silently losing
     * them during a subsequent save. Only the final assignment publishes. */
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, 0) != 4U) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = Text(tree, 0, "format", format, sizeof(format));
    if (status == UMI_STATUS_OK && strcmp(format, "umicom-media-creation-draft") != 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiJsonTreeMember(tree, 0, "version", &version_node);
    if (status == UMI_STATUS_OK) status = UmiJsonTreeInteger(tree, version_node, &version);
    if (status == UMI_STATUS_OK && version != 1) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiJsonTreeMember(tree, 0, "draft", &draft_node);
    if (status == UMI_STATUS_OK) status = ReadDraft(tree, draft_node, &candidate->draft);
    if (status == UMI_STATUS_OK)
        status = Text(tree, 0, "prompt", candidate->prompt, sizeof(candidate->prompt));
    if (status == UMI_STATUS_OK) status = UmiMediaBriefDocumentValidate(candidate);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) *out = *candidate;
    UmiJsonTreeDestroy(tree);
    umi_secret_clear(candidate, sizeof(*candidate));
    free(candidate);
    return status;
}
