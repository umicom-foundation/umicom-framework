/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_storyboard_document.c
 *
 * PURPOSE:
 *   Exercise bounded editable-storyboard round trips and unchanged outputs on malformed document admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/creative_workspace/media_storyboard.h"
#include "../../src/creative_workspace/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
static UmiMediaStoryboardDraft Draft(void)
{
    UmiMediaStoryboardDraft draft = {0};
    draft.count = 1U;
    strcpy(draft.scenes[0].title, "Opening");
    strcpy(draft.scenes[0].story, "A calm morning.");
    draft.scenes[0].duration_seconds = 10U;
    return draft;
}
static UmiStatus Refuse(const void *bytes, size_t size, const UmiCancellationToken *cancel)
{
    UmiMediaStoryboardDraft result, before;
    memset(&result, 0x5a, sizeof(result));
    memcpy(&before, &result, sizeof(before));
    UmiStatus status = UmiMediaStoryboardDraftImport(bytes, size, cancel, &result);
    CHECK(status != UMI_STATUS_OK);
    CHECK(memcmp(&result, &before, sizeof(result)) == 0);
    return status;
}
static void RoundTrip(const UmiMediaStoryboardDraft *draft, const char *name)
{
    UmiCreativeAsset *asset = NULL;
    UmiMediaStoryboardDraft restored;
    memset(&restored, 0x5a, sizeof(restored));
    CHECK(UmiMediaStoryboardDraftExport(draft, NULL, &asset) == UMI_STATUS_OK);
    const void *bytes = NULL;
    size_t size = 0U;
    CHECK(UmiCreativeAssetBytes(asset, &bytes, &size) == UMI_STATUS_OK);
    CHECK(size > 0U && size <= UMI_MEDIA_STORYBOARD_DOCUMENT_BYTES);
    CHECK(UmiMediaStoryboardDraftImport(bytes, size, NULL, &restored) == UMI_STATUS_OK);
    CHECK(restored.count == draft->count);
    for (size_t i = 0U; i < draft->count; ++i) {
        CHECK(strcmp(restored.scenes[i].title, draft->scenes[i].title) == 0);
        CHECK(strcmp(restored.scenes[i].story, draft->scenes[i].story) == 0);
        CHECK(restored.scenes[i].duration_seconds == draft->scenes[i].duration_seconds);
    }
    UmiMediaStoryboardScene blank = {0};
    for (size_t i = restored.count; i < UMI_MEDIA_STORYBOARD_MAX_SCENES; ++i)
        CHECK(memcmp(&restored.scenes[i], &blank, sizeof(blank)) == 0);
    /* Reopening text never turns unresolved content into generation authority. */
    UmiMediaStoryboard *plan = NULL;
    if (strcmp(name, "unresolved") == 0)
        CHECK(UmiMediaStoryboardCreate(&restored, NULL, 0U, &plan) == UMI_STATUS_NOT_FOUND);
    if (strcmp(name, "incomplete-tag") == 0 || strcmp(name, "empty") == 0)
        CHECK(UmiMediaStoryboardCreate(&restored, NULL, 0U, &plan) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(plan == NULL);
    UmiCreativeAssetDestroy(asset);
}
typedef struct BadDocument { const char *name; const char *json; } BadDocument;
static const BadDocument malformed[] = {
    {"root-array", "[]"},
    {"root-null", "null"},
    {"missing-scenes", "{\"format\":\"umicom-media-storyboard\",\"version\":1}"},
    {"unknown-root", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[],\"approved\":true}"},
    {"duplicate-format", "{\"format\":\"umicom-media-storyboard\",\"format\":\"umicom-media-storyboard\",\"scenes\":[]}"},
    {"future-version", "{\"format\":\"umicom-media-storyboard\",\"version\":2,\"scenes\":[]}"},
    {"fractional-version", "{\"format\":\"umicom-media-storyboard\",\"version\":1.0,\"scenes\":[]}"},
    {"wrong-format", "{\"format\":\"umicom-song-plan\",\"version\":1,\"scenes\":[]}"},
    {"scenes-object", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":{}}"},
    {"scene-number", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[1]}"},
    {"unknown-scene", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"B\",\"duration_seconds\":1,\"path\":\"private\"}]}"},
    {"duplicate-scene", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"title\":\"B\",\"duration_seconds\":1}]}"},
    {"fractional-duration", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"B\",\"duration_seconds\":1.0}]}"},
    {"negative-duration", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"B\",\"duration_seconds\":-1}]}"},
    {"zero-duration", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"B\",\"duration_seconds\":0}]}"},
    {"large-duration", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"B\",\"duration_seconds\":601}]}"},
    {"missing-story", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"duration_seconds\":1}]}"},
    {"title-number", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":1,\"story\":\"B\",\"duration_seconds\":1}]}"},
    {"blank-title", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\" \",\"story\":\"B\",\"duration_seconds\":1}]}"},
    {"blank-story", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\" \",\"duration_seconds\":1}]}"},
    {"surrogate", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"\\ud800\",\"duration_seconds\":1}]}"},
    {"decoded-nul", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"B\\u0000C\",\"duration_seconds\":1}]}"},
    {"control-story", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"B\\u0001\",\"duration_seconds\":1}]}"},
    {"trailing-value", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[]}false"},
    {"late-invalid-scene", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"B\",\"duration_seconds\":1},{\"title\":\"C\",\"story\":\"D\",\"duration_seconds\":0}]}"}
};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    if (strncmp(name, "writer-", 7U) == 0) {
        UmiCreativeWriter writer;
        UmiCreativeExport result = {0};
        UmiCreativeWriterInit(&writer, strcmp(name, "writer-exact") == 0 ? 3U : 2U);
        UmiCreativeWriterJson(&writer, strcmp(name, "writer-null-text") == 0 ? NULL : "x");
        if (strcmp(name, "writer-sticky") == 0) {
            UmiCreativeWriterText(&writer, "");
            UmiCreativeWriterNumber(&writer, 0U);
        }
        UmiStatus status = UmiCreativeWriterFinish(&writer, &result);
        if (strcmp(name, "writer-exact") == 0) {
            CHECK(status == UMI_STATUS_OK && result.size == 3U);
            CHECK(memcmp(result.bytes, "\"x\"", 4U) == 0);
        } else {
            CHECK(status == (strcmp(name, "writer-null-text") == 0 ?
                UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CAPACITY_EXCEEDED));
            CHECK(result.bytes == NULL && result.size == 0U);
        }
        UmiCreativeExportFree(&result);
        return 0;
    }
    for (size_t i = 0U; i < sizeof(malformed) / sizeof(malformed[0]); ++i)
        if (strcmp(name, malformed[i].name) == 0) {
            Refuse(malformed[i].json, strlen(malformed[i].json), NULL);
            return 0;
        }
    UmiMediaStoryboardDraft draft = Draft();
    UmiCreativeAsset *asset = NULL;
    if (strcmp(name, "bad-count") == 0) {
        draft.count = UMI_MEDIA_STORYBOARD_MAX_SCENES + 1U;
    } else if (strcmp(name, "unterminated-title") == 0) {
        memset(draft.scenes[0].title, 'T', sizeof(draft.scenes[0].title));
    } else if (strcmp(name, "unterminated-story") == 0) {
        memset(draft.scenes[0].story, 'S', sizeof(draft.scenes[0].story));
    } else if (strcmp(name, "draft-zero-duration") == 0) {
        draft.scenes[0].duration_seconds = 0U;
    } else if (strcmp(name, "draft-large-duration") == 0) {
        draft.scenes[0].duration_seconds = 601U;
    } else if (strcmp(name, "draft-invalid-utf8") == 0) {
        draft.scenes[0].story[0] = (char)0xc0;
        draft.scenes[0].story[1] = (char)0x80;
    } else if (strcmp(name, "occupied-output") == 0) {
        CHECK(UmiMediaStoryboardDraftExport(&draft, NULL, &asset) == UMI_STATUS_OK);
        UmiCreativeAsset *before = asset;
        CHECK(UmiMediaStoryboardDraftExport(&draft, NULL, &asset) == UMI_STATUS_INVALID_STATE);
        CHECK(asset == before);
        UmiCreativeAssetDestroy(asset);
        return 0;
    } else if (strcmp(name, "cancel-export") == 0 || strcmp(name, "cancel-import") == 0) {
        UmiCancellationToken *token = NULL;
        CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        if (strcmp(name, "cancel-export") == 0)
            CHECK(UmiMediaStoryboardDraftExport(&draft, token, &asset) == UMI_STATUS_CANCELLED && asset == NULL);
        else {
            const char json[] = "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[]}";
            CHECK(Refuse(json, sizeof(json) - 1U, token) == UMI_STATUS_CANCELLED);
        }
        umi_cancellation_token_destroy(token);
        return 0;
    } else if (strcmp(name, "null-arguments") == 0) {
        CHECK(UmiMediaStoryboardDraftValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMediaStoryboardDraftExport(NULL, NULL, &asset) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMediaStoryboardDraftExport(&draft, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMediaStoryboardDraftImport("{}", 2U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        Refuse(NULL, 1U, NULL);
        return 0;
    } else if (strcmp(name, "empty-input") == 0) {
        Refuse("", 0U, NULL); return 0;
    } else if (strcmp(name, "oversized-input") == 0) {
        size_t size = (size_t)UMI_MEDIA_STORYBOARD_DOCUMENT_BYTES + 1U;
        char *bytes = malloc(size);
        CHECK(bytes != NULL);
        memset(bytes, ' ', size);
        Refuse(bytes, size, NULL);
        free(bytes); return 0;
    } else if (strcmp(name, "excess-scenes") == 0) {
        char json[4096] = "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[";
        for (size_t i = 0U; i <= UMI_MEDIA_STORYBOARD_MAX_SCENES; ++i)
            strcat(json, i == 0U ? "{\"title\":\"A\",\"story\":\"B\",\"duration_seconds\":1}" :
                ",{\"title\":\"A\",\"story\":\"B\",\"duration_seconds\":1}");
        strcat(json, "]}");
        Refuse(json, strlen(json), NULL); return 0;
    } else if (strcmp(name, "raw-nul") == 0) {
        const char json[] = "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[]}\0";
        Refuse(json, sizeof(json) - 1U, NULL); return 0;
    } else if (strcmp(name, "raw-invalid-utf8") == 0) {
        const char json[] = "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"story\":\"\xc0\x80\",\"duration_seconds\":1}]}";
        Refuse(json, sizeof(json) - 1U, NULL); return 0;
    } else if (strcmp(name, "escaped-duplicate") == 0) {
        const char json[] = "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"scenes\":[{\"title\":\"A\",\"tit\\u006ce\":\"B\",\"duration_seconds\":1}]}";
        Refuse(json, sizeof(json) - 1U, NULL); return 0;
    } else {
        if (strcmp(name, "empty") == 0) memset(&draft, 0, sizeof(draft));
        else if (strcmp(name, "unresolved") == 0) strcpy(draft.scenes[0].story, "Show @{missing}.");
        else if (strcmp(name, "incomplete-tag") == 0) strcpy(draft.scenes[0].story, "Draft @{unfinished");
        else if (strcmp(name, "multiline") == 0) strcpy(draft.scenes[0].story, "Line one\r\nLine two\tcontinued.");
        else if (strcmp(name, "unicode") == 0) strcpy(draft.scenes[0].story, "Caf\xc3\xa9 \xe2\x98\x80");
        else if (strcmp(name, "escaped") == 0) strcpy(draft.scenes[0].story, "Say \"hello\" at C:\\notes.");
        else if (strcmp(name, "max-scenes") == 0) {
            draft.count = UMI_MEDIA_STORYBOARD_MAX_SCENES;
            for (size_t i = 1U; i < draft.count; ++i) draft.scenes[i] = draft.scenes[0];
        } else if (strcmp(name, "max-fields") == 0) {
            memset(draft.scenes[0].title, 'T', sizeof(draft.scenes[0].title) - 1U);
            memset(draft.scenes[0].story, '"', sizeof(draft.scenes[0].story) - 1U);
            draft.scenes[0].duration_seconds = 600U;
            draft.count = UMI_MEDIA_STORYBOARD_MAX_SCENES;
            for (size_t i = 1U; i < draft.count; ++i) draft.scenes[i] = draft.scenes[0];
        } else if (strcmp(name, "inactive-slots") == 0) {
            memset(&draft.scenes[1], 0xff, sizeof(draft.scenes) - sizeof(draft.scenes[0]));
        } else if (strcmp(name, "order") == 0) {
            draft.count = 2U;
            draft.scenes[1] = draft.scenes[0];
            strcpy(draft.scenes[1].title, "Closing");
            draft.scenes[1].duration_seconds = 20U;
            CHECK(UmiMediaStoryboardMove(&draft, 1U, 0U) == UMI_STATUS_OK);
        } else CHECK(strcmp(name, "roundtrip") == 0);
        RoundTrip(&draft, name);
        return 0;
    }
    CHECK(UmiMediaStoryboardDraftValidate(&draft) != UMI_STATUS_OK);
    CHECK(UmiMediaStoryboardDraftExport(&draft, NULL, &asset) != UMI_STATUS_OK && asset == NULL);
    return 0;
}
