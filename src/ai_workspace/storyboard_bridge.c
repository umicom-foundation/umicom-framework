/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/storyboard_bridge.c
 *
 * PURPOSE:
 *   Decode scene/asset metadata and reuse the existing Framework storyboard service.
 *
 * EDUCATIONAL OVERVIEW:
 *   Wire decoding is separate from scene rules. The canonical creative workspace
 *   owns scene validation and asset-reference binding; no parallel planner exists.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/storyboard_bridge.h"
#include <json-c/json.h>
#include <stdlib.h>
#include <string.h>
#ifdef UMICOM_STORYBOARD_LINKED
#include "umicom/creative_workspace/media_storyboard.h"

/* Copy exact UTF-8 scalar bytes; embedded NULs and over-capacity strings are errors. */
static bool StoryString(json_object *object, const char *key, char *out, size_t capacity)
{
    json_object *value = NULL;
    if (!json_object_object_get_ex(object, key, &value) || !json_object_is_type(value, json_type_string)) return false;
    int length = json_object_get_string_len(value);
    const char *text = json_object_get_string(value);
    if (length <= 0 || (size_t)length >= capacity || strlen(text) != (size_t)length) return false;
    memcpy(out, text, (size_t)length + 1U);
    return true;
}

/* Asset kind is declarative metadata, never a claim that a decoder saw its contents. */
static UmiCreativeAssetKind StoryKind(const char *kind)
{
    if (strcmp(kind, "document") == 0) return UMI_CREATIVE_ASSET_DOCUMENT;
    if (strcmp(kind, "image") == 0) return UMI_CREATIVE_ASSET_IMAGE;
    if (strcmp(kind, "video") == 0) return UMI_CREATIVE_ASSET_VIDEO;
    if (strcmp(kind, "audio") == 0) return UMI_CREATIVE_ASSET_AUDIO;
    return (UmiCreativeAssetKind)0;
}

/* Decode only structural fields; canonical Append/Create enforce scene and reference semantics. */
static UmiStatus StoryDecode(json_object *root, UmiMediaStoryboard **out)
{
    json_object *scenes = NULL, *assets = NULL;
    if (!json_object_object_get_ex(root, "scenes", &scenes) || !json_object_is_type(scenes, json_type_array) ||
        !json_object_object_get_ex(root, "assets", &assets) || !json_object_is_type(assets, json_type_array))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t sceneCount = json_object_array_length(scenes), assetCount = json_object_array_length(assets);
    if (sceneCount == 0U || sceneCount > UMI_MEDIA_STORYBOARD_MAX_SCENES ||
        assetCount > UMI_CREATIVE_LIBRARY_MAX_ASSETS) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiMediaStoryboardDraft *draft = calloc(1U, sizeof(*draft));
    UmiCreativeAssetLibraryEntry *entries = calloc(assetCount == 0U ? 1U : assetCount, sizeof(*entries));
    if (draft == NULL || entries == NULL) { free(draft); free(entries); return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; i < assetCount && status == UMI_STATUS_OK; ++i) {
        json_object *asset = json_object_array_get_idx(assets, i);
        char kind[32];
        if (!json_object_is_type(asset, json_type_object) ||
            !StoryString(asset, "id", entries[i].id, sizeof(entries[i].id)) ||
            !StoryString(asset, "label", entries[i].asset.label, sizeof(entries[i].asset.label)) ||
            !StoryString(asset, "kind", kind, sizeof(kind))) { status = UMI_STATUS_INVALID_ARGUMENT; break; }
        entries[i].asset.declared_kind = StoryKind(kind);
        if (entries[i].asset.declared_kind == (UmiCreativeAssetKind)0) status = UMI_STATUS_INVALID_ARGUMENT;
    }
    for (size_t i = 0U; i < sceneCount && status == UMI_STATUS_OK; ++i) {
        json_object *entry = json_object_array_get_idx(scenes, i), *duration = NULL;
        UmiMediaStoryboardScene scene = {0};
        if (!json_object_is_type(entry, json_type_object) ||
            !StoryString(entry, "title", scene.title, sizeof(scene.title)) ||
            !StoryString(entry, "story", scene.story, sizeof(scene.story)) ||
            !json_object_object_get_ex(entry, "duration_seconds", &duration) ||
            !json_object_is_type(duration, json_type_int) ||
            json_object_get_int64(duration) < 1 || json_object_get_int64(duration) > 600) {
            status = UMI_STATUS_INVALID_ARGUMENT; break;
        }
        scene.duration_seconds = (uint32_t)json_object_get_int64(duration);
        status = UmiMediaStoryboardAppend(draft, &scene);
    }
    if (status == UMI_STATUS_OK) status = UmiMediaStoryboardCreate(draft, entries, assetCount, out);
    free(entries); free(draft);
    return status;
}
#endif

/* Unlinked builds expose an unavailable capability, never simulated instructions. */
bool UmiAiStoryboardBridgeAvailable(void)
{
#ifdef UMICOM_STORYBOARD_LINKED
    return true;
#else
    return false;
#endif
}

/* Native compilation returns reference-aware text; it never reports generated image/video bytes. */
UmiStatus UmiAiStoryboardCompile(const char *input, size_t length, UmiAiWorkerWrite sink, void *context)
{
#ifdef UMICOM_STORYBOARD_LINKED
    if (sink == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    json_object *root = NULL, *reply = NULL;
    UmiMediaStoryboard *plan = NULL;
    UmiCreativeExport instructions = {0};
    UmiStatus status = UmiAiWorkerParseJson(input, length, &root);
    if (status == UMI_STATUS_OK) status = StoryDecode(root, &plan);
    if (status == UMI_STATUS_OK) status = UmiMediaStoryboardInstructions(plan, &instructions);
    if (status == UMI_STATUS_OK) {
        reply = json_object_new_object();
        json_object *text = json_object_new_string_len((const char *)instructions.bytes, (int)instructions.size);
        if (reply == NULL || text == NULL) { if (text != NULL) json_object_put(text); status = UMI_STATUS_OUT_OF_MEMORY; }
        else if (json_object_object_add(reply, "instructions", text) != 0) { json_object_put(text); status = UMI_STATUS_OUT_OF_MEMORY; }
        else {
            const char *output = json_object_to_json_string_ext(reply, JSON_C_TO_STRING_PLAIN);
            if (output == NULL || !sink(context, output, strlen(output))) status = UMI_STATUS_IO_ERROR;
        }
    }
    if (reply != NULL) json_object_put(reply);
    UmiCreativeExportFree(&instructions);
    UmiMediaStoryboardDestroy(plan);
    if (root != NULL) json_object_put(root);
    return status;
#else
    (void)input; (void)length; (void)sink; (void)context;
    return UMI_STATUS_UNAVAILABLE;
#endif
}
