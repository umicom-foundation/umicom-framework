/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/media_storyboard.c
 *
 * PURPOSE:
 *   Validate scene edits and bind tagged story references without reading or sending assets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/creative_workspace/media_storyboard.h"
#include "umicom/security/secrets.h"
#include "internal.h"
#include <stdlib.h>
#include <string.h>

typedef struct StoryAsset {
    char id[UMI_CREATIVE_ID_CAPACITY], label[UMI_CREATIVE_LABEL_CAPACITY];
    UmiCreativeAssetKind kind;
} StoryAsset;
struct UmiMediaStoryboard {
    UmiMediaStoryboardDraft draft;
    StoryAsset assets[UMI_CREATIVE_LIBRARY_MAX_ASSETS];
    size_t asset_count;
    size_t references[UMI_MEDIA_STORYBOARD_MAX_SCENES][UMI_MEDIA_STORYBOARD_MAX_REFERENCES];
    size_t counts[UMI_MEDIA_STORYBOARD_MAX_SCENES];
};
static UmiStatus ValidateScene(const UmiMediaStoryboardScene *scene)
{
    if (scene == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(scene->title, 0, sizeof(scene->title)) == NULL ||
        memchr(scene->story, 0, sizeof(scene->story)) == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!UmiCreativeTextValid(scene->title, sizeof(scene->title), false) ||
        scene->duration_seconds == 0U || scene->duration_seconds > 600U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Reuse Framework's strict scalar validation. Normalising whitespace in a
     * temporary copy permits paragraphs without weakening its control checks. */
    char single_line[UMI_MEDIA_STORYBOARD_STORY_CAPACITY];
    memcpy(single_line, scene->story, sizeof(single_line));
    for (size_t i = 0U; single_line[i] != 0; ++i)
        if (single_line[i] == '\n' || single_line[i] == '\r' || single_line[i] == '\t') single_line[i] = ' ';
    bool valid = UmiCreativeTextValid(single_line, sizeof(single_line), false);
    umi_secret_clear(single_line, sizeof(single_line));
    return valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static bool DraftValid(const UmiMediaStoryboardDraft *draft)
{ return draft != NULL && draft->count <= UMI_MEDIA_STORYBOARD_MAX_SCENES; }
UmiStatus UmiMediaStoryboardAppend(UmiMediaStoryboardDraft *draft, const UmiMediaStoryboardScene *scene)
{
    if (!DraftValid(draft)) return UMI_STATUS_INVALID_ARGUMENT;
    if (draft->count == UMI_MEDIA_STORYBOARD_MAX_SCENES) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = ValidateScene(scene);
    if (status == UMI_STATUS_OK) { draft->scenes[draft->count] = *scene; ++draft->count; }
    return status;
}
UmiStatus UmiMediaStoryboardUpdate(UmiMediaStoryboardDraft *draft, size_t index, const UmiMediaStoryboardScene *scene)
{
    if (!DraftValid(draft) || index >= draft->count) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ValidateScene(scene);
    if (status == UMI_STATUS_OK) draft->scenes[index] = *scene;
    return status;
}
UmiStatus UmiMediaStoryboardRemove(UmiMediaStoryboardDraft *draft, size_t index)
{
    if (!DraftValid(draft) || index >= draft->count) return UMI_STATUS_INVALID_ARGUMENT;
    memmove(&draft->scenes[index], &draft->scenes[index + 1U],
        (draft->count - index - 1U) * sizeof(draft->scenes[0]));
    --draft->count;
    umi_secret_clear(&draft->scenes[draft->count], sizeof(draft->scenes[0]));
    return UMI_STATUS_OK;
}
UmiStatus UmiMediaStoryboardMove(UmiMediaStoryboardDraft *draft, size_t from, size_t to)
{
    if (!DraftValid(draft) || from >= draft->count || to >= draft->count) return UMI_STATUS_INVALID_ARGUMENT;
    UmiMediaStoryboardScene scene = draft->scenes[from];
    if (from < to) memmove(&draft->scenes[from], &draft->scenes[from + 1U], (to - from) * sizeof(scene));
    if (to < from) memmove(&draft->scenes[to + 1U], &draft->scenes[to], (from - to) * sizeof(scene));
    draft->scenes[to] = scene;
    umi_secret_clear(&scene, sizeof(scene));
    return UMI_STATUS_OK;
}
static UmiStatus Bind(UmiMediaStoryboard *plan, size_t scene)
{
    const char *cursor = plan->draft.scenes[scene].story;
    while ((cursor = strstr(cursor, "@{")) != NULL) {
        cursor += 2;
        const char *end = strchr(cursor, '}');
        if (end == NULL) return UMI_STATUS_INVALID_ARGUMENT;
        size_t length = (size_t)(end - cursor);
        if (length == 0U || length >= UMI_CREATIVE_ID_CAPACITY) return UMI_STATUS_INVALID_ARGUMENT;
        char id[UMI_CREATIVE_ID_CAPACITY] = {0};
        memcpy(id, cursor, length);
        if (!UmiCreativeIdValid(id)) return UMI_STATUS_INVALID_ARGUMENT;
        size_t asset = 0U;
        while (asset < plan->asset_count && strcmp(id, plan->assets[asset].id) != 0) ++asset;
        if (asset == plan->asset_count) return UMI_STATUS_NOT_FOUND;
        size_t count = plan->counts[scene], ref = 0U;
        while (ref < count && plan->references[scene][ref] != asset) ++ref;
        if (ref == count) {
            if (count == UMI_MEDIA_STORYBOARD_MAX_REFERENCES) return UMI_STATUS_CAPACITY_EXCEEDED;
            plan->references[scene][count] = asset;
            ++plan->counts[scene];
        }
        cursor = end + 1;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiMediaStoryboardCreate(const UmiMediaStoryboardDraft *draft,
    const UmiCreativeAssetLibraryEntry *assets, size_t asset_count, UmiMediaStoryboard **out)
{
    if (!DraftValid(draft) || draft->count == 0U || out == NULL || *out != NULL ||
        asset_count > UMI_CREATIVE_LIBRARY_MAX_ASSETS || (asset_count != 0U && assets == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiMediaStoryboard *plan = calloc(1U, sizeof(*plan));
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    plan->draft = *draft;
    plan->asset_count = asset_count;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < asset_count; ++i) {
        if (!UmiCreativeIdValid(assets[i].id) ||
            !UmiCreativeTextValid(assets[i].asset.label, sizeof(assets[i].asset.label), false) ||
            assets[i].asset.declared_kind < UMI_CREATIVE_ASSET_DOCUMENT ||
            assets[i].asset.declared_kind > UMI_CREATIVE_ASSET_BINARY) { status = UMI_STATUS_INVALID_ARGUMENT; break; }
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(assets[i].id, assets[j].id) == 0) status = UMI_STATUS_ALREADY_EXISTS;
        memcpy(plan->assets[i].id, assets[i].id, sizeof(plan->assets[i].id));
        memcpy(plan->assets[i].label, assets[i].asset.label, sizeof(plan->assets[i].label));
        plan->assets[i].kind = assets[i].asset.declared_kind;
    }
    for (size_t i = 0U; status == UMI_STATUS_OK && i < draft->count; ++i) {
        status = ValidateScene(&draft->scenes[i]);
        if (status == UMI_STATUS_OK) status = Bind(plan, i);
    }
    if (status != UMI_STATUS_OK) { UmiMediaStoryboardDestroy(plan); return status; }
    *out = plan;
    return UMI_STATUS_OK;
}
void UmiMediaStoryboardDestroy(UmiMediaStoryboard *plan)
{ if (plan != NULL) { umi_secret_clear(plan, sizeof(*plan)); free(plan); } }
static void WriteScene(UmiCreativeWriter *writer, const UmiMediaStoryboard *plan, size_t index)
{
    const UmiMediaStoryboardScene *scene = &plan->draft.scenes[index];
    UmiCreativeWriterText(writer, "Scene "); UmiCreativeWriterNumber(writer, index + 1U);
    UmiCreativeWriterText(writer, ": "); UmiCreativeWriterText(writer, scene->title);
    UmiCreativeWriterText(writer, "\nRequested duration (seconds): ");
    UmiCreativeWriterNumber(writer, scene->duration_seconds);
    UmiCreativeWriterText(writer, "\nStory and direction:\n"); UmiCreativeWriterText(writer, scene->story);
    UmiCreativeWriterText(writer, "\nReferenced library assets (not uploaded):\n");
    const char *kinds[] = {"unknown", "document", "image", "video", "audio", "lyrics", "binary"};
    for (size_t i = 0U; i < plan->counts[index]; ++i) {
        const StoryAsset *asset = &plan->assets[plan->references[index][i]];
        UmiCreativeWriterText(writer, "@{"); UmiCreativeWriterText(writer, asset->id);
        UmiCreativeWriterText(writer, "} ["); UmiCreativeWriterText(writer, kinds[asset->kind]);
        UmiCreativeWriterText(writer, "] "); UmiCreativeWriterText(writer, asset->label);
        UmiCreativeWriterText(writer, "\n");
    }
    UmiCreativeWriterText(writer, "\n");
}
static UmiStatus Render(const UmiMediaStoryboard *plan, size_t first, size_t count, UmiCreativeExport *out)
{
    if (plan == NULL || out == NULL || out->bytes != NULL || out->size != 0U ||
        first >= plan->draft.count || count > plan->draft.count - first) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeWriter writer;
    UmiCreativeWriterInit(&writer, 131072U);
    UmiCreativeWriterText(&writer, "Umicom Media storyboard instructions\n"
        "Reference tags identify library assets; no files are attached by this text.\n"
        "Review provider capabilities and attach approved references separately.\n"
        "Composition: use the canonical Umicom SVG watermark in the final export.\n\n");
    for (size_t i = first; i < first + count; ++i) WriteScene(&writer, plan, i);
    return UmiCreativeWriterFinish(&writer, out);
}
UmiStatus UmiMediaStoryboardInstructions(const UmiMediaStoryboard *plan, UmiCreativeExport *out)
{ return Render(plan, 0U, plan != NULL ? plan->draft.count : 0U, out); }
UmiStatus UmiMediaStoryboardSceneInstructions(const UmiMediaStoryboard *plan, size_t index, UmiCreativeExport *out)
{ return Render(plan, index, 1U, out); }
