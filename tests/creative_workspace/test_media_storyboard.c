/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_storyboard.c
 *
 * PURPOSE:
 *   Exercise ordered scene edits, reference validation and immutable instruction ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/creative_workspace/media_storyboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
static UmiMediaStoryboardScene Scene(const char *title, const char *story)
{
    UmiMediaStoryboardScene scene = {0};
    CHECK(strlen(title) < sizeof(scene.title) && strlen(story) < sizeof(scene.story));
    memcpy(scene.title, title, strlen(title) + 1U);
    memcpy(scene.story, story, strlen(story) + 1U); scene.duration_seconds = 10U;
    return scene;
}
static void Asset(UmiCreativeAssetLibraryEntry *asset, const char *id, UmiCreativeAssetKind kind)
{
    memset(asset, 0, sizeof(*asset));
    (void)snprintf(asset->id, sizeof(asset->id), "%s", id);
    (void)snprintf(asset->asset.label, sizeof(asset->asset.label), "%s reference", id);
    asset->asset.declared_kind = kind;
    (void)snprintf(asset->asset.source_path, sizeof(asset->asset.source_path), "private-local-source");
}
static UmiStatus Create(const char *story, const UmiCreativeAssetLibraryEntry *assets,
                        size_t count, UmiMediaStoryboard **plan)
{
    UmiMediaStoryboardDraft draft = {0}; UmiMediaStoryboardScene scene = Scene("Opening", story);
    CHECK(UmiMediaStoryboardAppend(&draft, &scene) == UMI_STATUS_OK);
    return UmiMediaStoryboardCreate(&draft, assets, count, plan);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name = argv[1];
    UmiCreativeAssetLibraryEntry *assets = calloc(UMI_CREATIVE_LIBRARY_MAX_ASSETS, sizeof(*assets));
    CHECK(assets != NULL);
    Asset(&assets[0], "avatar", UMI_CREATIVE_ASSET_IMAGE);
    Asset(&assets[1], "voice", UMI_CREATIVE_ASSET_AUDIO);
    UmiMediaStoryboard *plan = NULL; UmiCreativeExport output = {0};
    if (!strcmp(name, "references") || !strcmp(name, "immutable")) {
        CHECK(Create("Let @{avatar} introduce the coast. Use @{voice} for narration. Return to @{avatar}.", assets, 2U, &plan) == UMI_STATUS_OK);
        if (!strcmp(name, "immutable")) memset(assets, 0, 2U * sizeof(*assets));
        CHECK(UmiMediaStoryboardInstructions(plan, &output) == UMI_STATUS_OK);
        const char *text = (const char *)output.bytes;
        CHECK(output.size == strlen(text));
        CHECK(strstr(text, "@{avatar} [image] avatar reference") && strstr(text, "@{voice} [audio] voice reference"));
        const char *bound = strstr(text, "@{avatar} [image]");
        CHECK(strstr(bound + 1, "@{avatar} [image]") == NULL);
        CHECK(strstr(text, "private-local-source") == NULL);
        CHECK(strstr(text, "not uploaded") && strstr(text, "Umicom SVG watermark"));
    } else if (!strcmp(name, "edits")) {
        UmiMediaStoryboardDraft draft = {0}, before;
        UmiMediaStoryboardScene first = Scene("First", "Open the scene."), second = Scene("Second", "Close the scene.");
        CHECK(UmiMediaStoryboardAppend(&draft, &first) == UMI_STATUS_OK);
        CHECK(UmiMediaStoryboardAppend(&draft, &second) == UMI_STATUS_OK);
        CHECK(UmiMediaStoryboardMove(&draft, 1U, 0U) == UMI_STATUS_OK);
        CHECK(!strcmp(draft.scenes[0].title, "Second") && !strcmp(draft.scenes[1].title, "First"));
        before = draft; second.duration_seconds = 0U;
        CHECK(UmiMediaStoryboardUpdate(&draft, 0U, &second) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&before, &draft, sizeof(draft)));
        CHECK(UmiMediaStoryboardMove(&draft, 0U, 1U) == UMI_STATUS_OK);
        CHECK(UmiMediaStoryboardRemove(&draft, 0U) == UMI_STATUS_OK);
        CHECK(draft.count == 1U && !strcmp(draft.scenes[0].title, "Second"));
        UmiMediaStoryboardScene zero = {0}; CHECK(!memcmp(&draft.scenes[1], &zero, sizeof(zero)));
        first = Scene("Revised", "A replacement story.");
        CHECK(UmiMediaStoryboardUpdate(&draft, 0U, &first) == UMI_STATUS_OK);
        CHECK(!strcmp(draft.scenes[0].story, "A replacement story."));
    } else if (!strcmp(name, "unknown")) {
        CHECK(Create("@{missing}", assets, 2U, &plan) == UMI_STATUS_NOT_FOUND && plan == NULL);
        CHECK(Create("@{Avatar}", assets, 2U, &plan) == UMI_STATUS_NOT_FOUND && plan == NULL);
        CHECK(Create("Contact artist@example.test; ordinary @ remains text.", NULL, 0U, &plan) == UMI_STATUS_OK);
    } else if (!strcmp(name, "malformed")) {
        const char *bad[] = {"@{}", "@{avatar", "@{two words}", "@{@{avatar}}", "@{../avatar}"};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
            CHECK(Create(bad[i], assets, 2U, &plan) == UMI_STATUS_INVALID_ARGUMENT && plan == NULL);
    } else if (!strcmp(name, "duplicate")) {
        assets[1] = assets[0];
        CHECK(Create("@{avatar}", assets, 2U, &plan) == UMI_STATUS_ALREADY_EXISTS && plan == NULL);
        memset(assets[0].id, 'a', sizeof(assets[0].id));
        CHECK(Create("A scene", assets, 1U, &plan) == UMI_STATUS_INVALID_ARGUMENT && plan == NULL);
    } else if (!strcmp(name, "bounds")) {
        UmiMediaStoryboardDraft draft = {0}; UmiMediaStoryboardScene scene = Scene("Title", "Story");
        CHECK(UmiMediaStoryboardCreate(&draft, NULL, 0U, &plan) == UMI_STATUS_INVALID_ARGUMENT);
        memset(scene.story, 'x', sizeof(scene.story));
        CHECK(UmiMediaStoryboardAppend(&draft, &scene) == UMI_STATUS_CAPACITY_EXCEEDED && draft.count == 0U);
        scene.story[sizeof(scene.story) - 1U] = 0;
        CHECK(UmiMediaStoryboardAppend(&draft, &scene) == UMI_STATUS_OK);
        UmiMediaStoryboardDraft before = draft;
        CHECK(UmiMediaStoryboardRemove(&draft, SIZE_MAX) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMediaStoryboardMove(&draft, 0U, 1U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(&draft, &before, sizeof(draft)));
        CHECK(UmiMediaStoryboardCreate(&draft, NULL, 1U, &plan) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(name, "unicode")) {
        UmiMediaStoryboardDraft draft = {0};
        UmiMediaStoryboardScene scene = Scene("Caf\xc3\xa9", "A coast.\nNarrator:\t\xe6\xb5\xb7\xe5\xb2\xb8");
        CHECK(UmiMediaStoryboardAppend(&draft, &scene) == UMI_STATUS_OK);
        CHECK(UmiMediaStoryboardCreate(&draft, NULL, 0U, &plan) == UMI_STATUS_OK);
        const char *bad[] = {"\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80", "bad\x1b", "\n\t "};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
            scene = Scene("Title", bad[i]);
            CHECK(UmiMediaStoryboardUpdate(&draft, 0U, &scene) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (!strcmp(name, "limits")) {
        UmiMediaStoryboardDraft draft = {0}; UmiMediaStoryboardScene scene = Scene("Title", "A scene");
        for (size_t i = 0U; i < UMI_MEDIA_STORYBOARD_MAX_SCENES; ++i)
            CHECK(UmiMediaStoryboardAppend(&draft, &scene) == UMI_STATUS_OK);
        CHECK(UmiMediaStoryboardAppend(&draft, &scene) == UMI_STATUS_CAPACITY_EXCEEDED);
        char story[256] = {0}; size_t length = 0U;
        for (size_t i = 0U; i < 9U; ++i) {
            char id[16]; (void)snprintf(id, sizeof(id), "asset%zu", i); Asset(&assets[i], id, UMI_CREATIVE_ASSET_IMAGE);
            int n = snprintf(story + length, sizeof(story) - length, "@{%s} ", id); CHECK(n > 0); length += (size_t)n;
            if (i == 7U) { CHECK(Create(story, assets, 8U, &plan) == UMI_STATUS_OK); UmiMediaStoryboardDestroy(plan); plan = NULL; }
        }
        CHECK(Create(story, assets, 9U, &plan) == UMI_STATUS_CAPACITY_EXCEEDED && plan == NULL);
    } else if (!strcmp(name, "output")) {
        CHECK(Create("One scene", NULL, 0U, &plan) == UMI_STATUS_OK);
        CHECK(UmiMediaStoryboardSceneInstructions(plan, 1U, &output) == UMI_STATUS_INVALID_ARGUMENT && output.bytes == NULL);
        CHECK(UmiMediaStoryboardSceneInstructions(plan, 0U, &output) == UMI_STATUS_OK);
        unsigned char *bytes = output.bytes; size_t size = output.size;
        CHECK(UmiMediaStoryboardInstructions(plan, &output) != UMI_STATUS_OK && output.bytes == bytes && output.size == size);
        CHECK(strstr((char *)output.bytes, "Scene 1: Opening") && strstr((char *)output.bytes, "One scene"));
    } else CHECK(0);
    UmiCreativeExportFree(&output); UmiMediaStoryboardDestroy(plan); free(assets); return 0;
}
