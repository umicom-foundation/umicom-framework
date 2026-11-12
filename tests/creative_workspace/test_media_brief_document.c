/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_media_brief_document.c
 *
 * PURPOSE:
 *   Verify editable creation recovery, independent reviewed text and failure-atomic document admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/creative_workspace/media_brief_document.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
typedef struct BadDocument { const char *name, *json; } BadDocument;
static const BadDocument bad_documents[] = {
    {"root-array", "[]"},
    {"root-null", "null"},
    {"missing-prompt", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"}}"},
    {"unknown-root", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\",\"ready\":true}"},
    {"duplicate-root", "{\"format\":\"umicom-media-creation-draft\",\"format\":\"other\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"}"},
    {"escaped-duplicate", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\",\"pro\\u006dpt\":\"hidden\"}"},
    {"future-version", "{\"format\":\"umicom-media-creation-draft\",\"version\":2,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"}"},
    {"fractional-version", "{\"format\":\"umicom-media-creation-draft\",\"version\":1.5,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"}"},
    {"wrong-format", "{\"format\":\"umicom-media-storyboard\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"}"},
    {"draft-array", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":[],\"prompt\":\"\"}"},
    {"unknown-draft", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\",\"model\":\"x\"},\"prompt\":\"\"}"},
    {"missing-direction", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\"},\"prompt\":\"\"}"},
    {"kind-number", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":2,\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"}"},
    {"kind-unknown", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"audio\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"}"},
    {"kind-case", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"Video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"}"},
    {"duplicate-source", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"source\":\"hidden\",\"direction\":\"\"},\"prompt\":\"\"}"},
    {"title-number", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":1,\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"}"},
    {"source-null", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":null,\"direction\":\"\"},\"prompt\":\"\"}"},
    {"prompt-array", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":[]}"},
    {"decoded-nul", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"a\\u0000b\"}"},
    {"prompt-control", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"a\\u0001b\"}"},
    {"surrogate", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\\ud800\"}"},
    {"trailing-value", "{\"format\":\"umicom-media-creation-draft\",\"version\":1,\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"\"} true"},
};
static UmiMediaBriefDocument Sample(void)
{
    UmiMediaBriefDocument document = {0};
    document.draft.kind = UMI_MEDIA_BRIEF_VIDEO;
    strcpy(document.draft.title, "Harbour");
    strcpy(document.draft.source, "A boat arrives before dawn.");
    strcpy(document.draft.direction, "Keep the narration calm.");
    strcpy(document.prompt, "An independently edited brief for review.");
    return document;
}
static void Equal(const UmiMediaBriefDocument *a, const UmiMediaBriefDocument *b)
{
    CHECK(a->draft.kind == b->draft.kind);
    CHECK(strcmp(a->draft.title, b->draft.title) == 0);
    CHECK(strcmp(a->draft.source, b->draft.source) == 0);
    CHECK(strcmp(a->draft.direction, b->draft.direction) == 0);
    CHECK(strcmp(a->prompt, b->prompt) == 0);
}
static void RoundTrip(UmiMediaBriefDocument *source)
{
    UmiCreativeAsset *asset = NULL;
    const void *bytes = NULL;
    size_t size = 0U;
    UmiMediaBriefDocument before, restored;
    memcpy(&before, source, sizeof(before));
    memset(&restored, 0x5a, sizeof(restored));
    CHECK(UmiMediaBriefDocumentExport(source, NULL, &asset) == UMI_STATUS_OK);
    CHECK(UmiCreativeAssetBytes(asset, &bytes, &size) == UMI_STATUS_OK);
    CHECK(size < UMI_MEDIA_BRIEF_DOCUMENT_BYTES);
    CHECK(UmiMediaBriefDocumentImport(bytes, size, NULL, &restored) == UMI_STATUS_OK);
    Equal(source, &restored);
    CHECK(memcmp(&before, source, sizeof(before)) == 0);
    /* Import owns its text after the encoded input is destroyed. */
    UmiCreativeAssetDestroy(asset);
    Equal(source, &restored);
}
static void Refuse(const void *bytes, size_t size)
{
    UmiMediaBriefDocument out = Sample(), before;
    memcpy(&before, &out, sizeof(before));
    CHECK(UmiMediaBriefDocumentImport(bytes, size, NULL, &out) != UMI_STATUS_OK);
    CHECK(memcmp(&before, &out, sizeof(before)) == 0);
}
static void Fill(char *text, size_t capacity)
{ memset(text, 'q', capacity - 1U); text[capacity - 1U] = '\0'; }
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    UmiMediaBriefDocument document = Sample();
    if (strcmp(name, "roundtrip") == 0) RoundTrip(&document);
    else if (strcmp(name, "kinds") == 0) {
        for (int kind = UMI_MEDIA_BRIEF_STORYBOARD; kind <= UMI_MEDIA_BRIEF_MUSIC; ++kind) {
            document.draft.kind = (UmiMediaBriefKind)kind;
            RoundTrip(&document);
        }
    } else if (strcmp(name, "empty") == 0) {
        memset(&document, 0, sizeof(document)); RoundTrip(&document);
        UmiMediaBrief *brief = NULL;
        CHECK(UmiMediaBriefCreate(&document.draft, &brief) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(brief == NULL);
    } else if (strcmp(name, "prompt-only") == 0) {
        memset(&document.draft, 0, sizeof(document.draft)); RoundTrip(&document);
    } else if (strcmp(name, "stale-prompt") == 0) {
        strcpy(document.draft.source, "A new source with no connection to the previous prompt.");
        RoundTrip(&document);
    } else if (strcmp(name, "whitespace") == 0) {
        strcpy(document.draft.title, " \t"); strcpy(document.draft.source, "\r\n");
        strcpy(document.prompt, "\t\n"); RoundTrip(&document);
    } else if (strcmp(name, "unicode") == 0) {
        strcpy(document.draft.title, "Caf\xc3\xa9");
        strcpy(document.prompt, "Dawn \xe2\x98\x80 / \xf0\x9f\x8c\x8a"); RoundTrip(&document);
    } else if (strcmp(name, "escaped") == 0) {
        strcpy(document.draft.source, "Quote \"one\"; folder C:\\drafts.\r\nNext\tline.");
        strcpy(document.prompt, "\\review\\ \"exact text\""); RoundTrip(&document);
    } else if (strcmp(name, "max-fields") == 0) {
        Fill(document.draft.title, sizeof(document.draft.title));
        Fill(document.draft.source, sizeof(document.draft.source));
        Fill(document.draft.direction, sizeof(document.draft.direction));
        Fill(document.prompt, sizeof(document.prompt)); RoundTrip(&document);
    } else if (strcmp(name, "ignored-tail") == 0) {
        document.prompt[sizeof(document.prompt) - 1U] = 'x'; RoundTrip(&document);
    } else if (strcmp(name, "invalid-kind") == 0) {
        UmiCreativeAsset *out = NULL;
        document.draft.kind = (UmiMediaBriefKind)99;
        CHECK(UmiMediaBriefDocumentExport(&document, NULL, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(out == NULL);
    } else if (strcmp(name, "unterminated-fields") == 0 ||
               strcmp(name, "malformed-fields") == 0 ||
               strcmp(name, "control-fields") == 0) {
        for (size_t i = 0U; i < 4U; ++i) {
            document = Sample();
            char *fields[] = {document.draft.title, document.draft.source,
                document.draft.direction, document.prompt};
            size_t sizes[] = {sizeof(document.draft.title), sizeof(document.draft.source),
                sizeof(document.draft.direction), sizeof(document.prompt)};
            if (strcmp(name, "unterminated-fields") == 0) memset(fields[i], 'x', sizes[i]);
            else if (strcmp(name, "malformed-fields") == 0) strcpy(fields[i], "\xc0\xaf");
            else strcpy(fields[i], "a\001b");
            UmiCreativeAsset *out = NULL;
            CHECK(UmiMediaBriefDocumentExport(&document, NULL, &out) != UMI_STATUS_OK);
            CHECK(out == NULL);
        }
    } else if (strcmp(name, "occupied-output") == 0) {
        UmiCreativeAsset *out = NULL;
        CHECK(UmiMediaBriefDocumentExport(&document, NULL, &out) == UMI_STATUS_OK);
        UmiCreativeAsset *before = out;
        CHECK(UmiMediaBriefDocumentExport(&document, NULL, &out) == UMI_STATUS_INVALID_STATE);
        CHECK(out == before); UmiCreativeAssetDestroy(out);
    } else if (strcmp(name, "cancel-export") == 0 || strcmp(name, "cancel-import") == 0) {
        UmiCancellationToken *token = NULL;
        CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        umi_cancellation_token_request(token);
        UmiCreativeAsset *out = NULL;
        if (strcmp(name, "cancel-export") == 0) {
            CHECK(UmiMediaBriefDocumentExport(&document, token, &out) == UMI_STATUS_CANCELLED);
            CHECK(out == NULL);
        } else {
            CHECK(UmiMediaBriefDocumentExport(&document, NULL, &out) == UMI_STATUS_OK);
            const void *bytes = NULL; size_t size = 0U;
            CHECK(UmiCreativeAssetBytes(out, &bytes, &size) == UMI_STATUS_OK);
            UmiMediaBriefDocument before; memcpy(&before, &document, sizeof(before));
            CHECK(UmiMediaBriefDocumentImport(bytes, size, token, &document) == UMI_STATUS_CANCELLED);
            CHECK(memcmp(&before, &document, sizeof(before)) == 0);
        }
        UmiCreativeAssetDestroy(out); umi_cancellation_token_destroy(token);
    } else if (strcmp(name, "null-arguments") == 0) {
        UmiCreativeAsset *out = NULL;
        CHECK(UmiMediaBriefDocumentValidate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMediaBriefDocumentExport(NULL, NULL, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMediaBriefDocumentExport(&document, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiMediaBriefDocumentImport("{}", 2U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        Refuse(NULL, 1U);
    } else if (strcmp(name, "empty-input") == 0) Refuse("", 0U);
    else if (strcmp(name, "oversized-input") == 0) {
        size_t size = UMI_MEDIA_BRIEF_DOCUMENT_BYTES + 1U;
        char *bytes = malloc(size); CHECK(bytes != NULL); memset(bytes, ' ', size);
        Refuse(bytes, size); free(bytes);
    } else if (strcmp(name, "raw-nul") == 0) {
        const char bytes[] = {'{','}',0,' '}; Refuse(bytes, sizeof(bytes));
    } else if (strcmp(name, "raw-invalid-utf8") == 0) {
        const char bytes[] = "\"\xc0\xaf\""; Refuse(bytes, sizeof(bytes) - 1U);
    } else if (strcmp(name, "oversized-prompt") == 0) {
        const char prefix[] = "{\"format\":\"umicom-media-creation-draft\",\"version\":1,"
            "\"draft\":{\"kind\":\"video\",\"title\":\"\",\"source\":\"\",\"direction\":\"\"},\"prompt\":\"";
        size_t size = sizeof(prefix) - 1U + UMI_MEDIA_BRIEF_PROMPT_CAPACITY + 2U;
        char *bytes = malloc(size); CHECK(bytes != NULL);
        memcpy(bytes, prefix, sizeof(prefix) - 1U);
        memset(bytes + sizeof(prefix) - 1U, 'x', UMI_MEDIA_BRIEF_PROMPT_CAPACITY);
        memcpy(bytes + size - 2U, "\"}", 2U); Refuse(bytes, size); free(bytes);
    } else {
        size_t i = 0U;
        while (i < sizeof(bad_documents) / sizeof(bad_documents[0]) &&
            strcmp(name, bad_documents[i].name) != 0) ++i;
        CHECK(i < sizeof(bad_documents) / sizeof(bad_documents[0]));
        Refuse(bad_documents[i].json, strlen(bad_documents[i].json));
    }
    return 0;
}
