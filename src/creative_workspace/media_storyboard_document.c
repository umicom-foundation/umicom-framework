/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/media_storyboard_document.c
 *
 * PURPOSE:
 *   Preserve editable scene inputs in bounded versioned JSON without restoring generation authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/creative_workspace/media_storyboard.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/security/secrets.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiMediaStoryboardDraftExport(const UmiMediaStoryboardDraft *draft,
    const UmiCancellationToken *cancel, UmiCreativeAsset **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*out != NULL) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiMediaStoryboardDraftValidate(draft);
    if (status != UMI_STATUS_OK) return status;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    UmiCreativeWriter writer;
    UmiCreativeExport document = {0};
    UmiCreativeWriterInit(&writer, UMI_MEDIA_STORYBOARD_DOCUMENT_BYTES);
    UmiCreativeWriterText(&writer,
        "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[");
    for (size_t i = 0U; i < draft->count && writer.status == UMI_STATUS_OK; ++i) {
        if (umi_cancellation_token_is_requested(cancel)) {
            writer.status = UMI_STATUS_CANCELLED;
            break;
        }
        const UmiMediaStoryboardScene *scene = &draft->scenes[i];
        UmiCreativeWriterText(&writer, i == 0U ? "{\"title\":" : ",{\"title\":");
        UmiCreativeWriterJson(&writer, scene->title);
        UmiCreativeWriterText(&writer, ",\"story\":");
        UmiCreativeWriterJson(&writer, scene->story);
        UmiCreativeWriterText(&writer, ",\"duration_seconds\":");
        UmiCreativeWriterNumber(&writer, scene->duration_seconds);
        UmiCreativeWriterText(&writer, "}");
    }
    UmiCreativeWriterText(&writer, "]}\n");
    status = UmiCreativeWriterFinish(&writer, &document);
    if (status == UMI_STATUS_OK)
        status = UmiCreativeAssetCapture("Umicom Media storyboard",
            UMI_CREATIVE_ASSET_DOCUMENT, document.bytes, document.size, cancel, out);
    if (document.bytes != NULL) umi_secret_clear(document.bytes, document.size);
    UmiCreativeExportFree(&document);
    return status;
}
static UmiStatus Text(const UmiJsonTree *tree, int object, const char *key,
    char *out, size_t capacity)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeText(tree, node, out, capacity) : status;
}
static UmiStatus Integer(const UmiJsonTree *tree, int object, const char *key,
    int64_t *out)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, object, key, &node);
    return status == UMI_STATUS_OK ? UmiJsonTreeInteger(tree, node, out) : status;
}
static UmiStatus ReadScene(const UmiJsonTree *tree, int node,
    UmiMediaStoryboardDraft *draft)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, node) != 3U) return UMI_STATUS_PARSE_ERROR;
    UmiMediaStoryboardScene scene = {0};
    int64_t duration = 0;
    UmiStatus status = Text(tree, node, "title", scene.title, sizeof(scene.title));
    if (status == UMI_STATUS_OK)
        status = Text(tree, node, "story", scene.story, sizeof(scene.story));
    if (status == UMI_STATUS_OK) status = Integer(tree, node, "duration_seconds", &duration);
    if (status == UMI_STATUS_OK && (duration < 1 || duration > 600))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK) {
        scene.duration_seconds = (uint32_t)duration;
        /* Append owns the scene text rules and complete-draft mutation. */
        status = UmiMediaStoryboardAppend(draft, &scene);
    }
    umi_secret_clear(&scene, sizeof(scene));
    return status;
}
UmiStatus UmiMediaStoryboardDraftImport(const void *bytes, size_t size,
    const UmiCancellationToken *cancel, UmiMediaStoryboardDraft *out)
{
    if (bytes == NULL || size == 0U || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {UMI_MEDIA_STORYBOARD_DOCUMENT_BYTES, 256U, 4U};
    UmiStatus status = UmiJsonTreeCreate(bytes, size, &limits, cancel, &tree);
    if (status != UMI_STATUS_OK) return status;
    UmiMediaStoryboardDraft *candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) {
        UmiJsonTreeDestroy(tree);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    char format[32] = {0};
    int64_t version = 0;
    int scenes = -1;
    /* Exact shapes make unsupported editing intent an error, rather than
     * silently accepting and discarding a future document's fields. */
    if (UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT ||
        UmiJsonTreeCount(tree, 0) != 3U) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = Text(tree, 0, "format", format, sizeof(format));
    if (status == UMI_STATUS_OK && strcmp(format, "umicom-media-storyboard") != 0)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = Integer(tree, 0, "version", &version);
    if (status == UMI_STATUS_OK && version != 1) status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiJsonTreeMember(tree, 0, "scenes", &scenes);
    if (status == UMI_STATUS_OK && UmiJsonTreeKind(tree, scenes) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && UmiJsonTreeCount(tree, scenes) > UMI_MEDIA_STORYBOARD_MAX_SCENES)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (int scene = status == UMI_STATUS_OK ? UmiJsonTreeFirst(tree, scenes) : -1;
         scene >= 0 && status == UMI_STATUS_OK; scene = UmiJsonTreeNext(tree, scene))
        status = ReadScene(tree, scene, candidate);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) *out = *candidate;
    UmiJsonTreeDestroy(tree);
    umi_secret_clear(candidate, sizeof(*candidate));
    free(candidate);
    return status;
}
