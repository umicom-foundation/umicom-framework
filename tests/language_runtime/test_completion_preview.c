/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_completion_preview.c
 * PURPOSE: Verify complete completion text, caret translation and refusal before publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/completion_preview.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
#define RANGE "{\"start\":{\"line\":1,\"character\":0},\"end\":{\"line\":1,\"character\":2}}"
#define IMPORT "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":0}}"
#define BEFORE "{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":2}}"
#define AFTER "{\"start\":{\"line\":2,\"character\":0},\"end\":{\"line\":2,\"character\":3}}"
#define PRIMARY "\"textEdit\":{\"range\":" RANGE ",\"newText\":\"puts\"}"
typedef struct PreviewCase
{
    const char *name, *json, *source, *result;
    size_t caret, begin, end, result_caret, edits;
    UmiStatus status;
} PreviewCase;
static const PreviewCase cases[] = {
    {"replace", "[{\"label\":\"puts\"," PRIMARY "}]", "//\nab\nend", "//\nputs\nend", 4, 3, 5, 7, 1,
     UMI_STATUS_OK},
    {"fallback", "[{\"label\":\"puts\"}]", "//\nab\nend", "//\nputs\nend", 4, 3, 5, 7, 1, UMI_STATUS_OK},
    {"insert", "[{\"label\":\"puts\"}]", "//\nab\nend", "//\naputsb\nend", 4, 4, 4, 8, 1, UMI_STATUS_OK},
    {"import",
     "[{\"label\":\"puts\"," PRIMARY ",\"additionalTextEdits\":[{\"range\":" IMPORT
     ",\"newText\":\"#inc\\n\"}]}]",
     "//\nab\nend", "#inc\n//\nputs\nend", 4, 3, 5, 12, 2, UMI_STATUS_OK},
    {"preceding-delete",
     "[{\"label\":\"puts\"," PRIMARY ",\"additionalTextEdits\":[{\"range\":" BEFORE ",\"newText\":\"\"}]}]",
     "//\nab\nend", "\nputs\nend", 4, 3, 5, 5, 2, UMI_STATUS_OK},
    {"following",
     "[{\"label\":\"puts\"," PRIMARY ",\"additionalTextEdits\":[{\"range\":" AFTER
     ",\"newText\":\"tail\"}]}]",
     "//\nab\nend", "//\nputs\ntail", 4, 3, 5, 7, 2, UMI_STATUS_OK},
    {"mixed-order",
     "[{\"label\":\"puts\"," PRIMARY ",\"additionalTextEdits\":[{\"range\":" AFTER
     ",\"newText\":\"tail\"},{\"range\":" BEFORE ",\"newText\":\"#inc\"}]}]",
     "//\nab\nend", "#inc\nputs\ntail", 4, 3, 5, 9, 3, UMI_STATUS_OK},
    {"delete", "[{\"label\":\"remove\",\"insertText\":\"\"}]", "//\nab\nend", "//\n\nend", 4, 3, 5, 3, 1,
     UMI_STATUS_OK},
    {"unicode", "[{\"label\":\"\\ud83d\\ude00\"}]", "//\nab\nend", "//\n\xf0\x9f\x98\x80\nend", 4, 3, 5, 7, 1,
     UMI_STATUS_OK},
    {"empty", "[{\"label\":\"puts\"}]", "", "puts", 0, 0, 0, 4, 1, UMI_STATUS_OK},
    {"crlf", "[{\"label\":\"puts\"}]", "//\r\nab\r\n", "//\r\nputs\r\n", 5, 4, 6, 8, 1, UMI_STATUS_OK},
    {"bad-source", "[{\"label\":\"puts\"}]", "a\xff", NULL, 0, 0, 0, 0, 0, UMI_STATUS_PARSE_ERROR},
    {"cursor-split", "[{\"label\":\"puts\"}]", "\xf0\x9f\x98\x80", NULL, 1, 0, 4, 0, 0,
     UMI_STATUS_INVALID_ARGUMENT},
    {"range-split", "[{\"label\":\"puts\"}]", "\xf0\x9f\x98\x80", NULL, 4, 1, 4, 0, 0,
     UMI_STATUS_INVALID_ARGUMENT},
    {"multiline", "[{\"label\":\"puts\"}]", "a\nb", NULL, 1, 0, 3, 0, 0, UMI_STATUS_INVALID_ARGUMENT},
    {"overlap",
     "[{\"label\":\"puts\"," PRIMARY ",\"additionalTextEdits\":[{\"range\":" RANGE ",\"newText\":\"bad\"}]}]",
     "//\nab\nend", NULL, 4, 3, 5, 0, 0, UMI_STATUS_INVALID_STATE},
    {"snippet", "[{\"label\":\"x\",\"insertTextFormat\":2}]", "a", NULL, 1, 0, 1, 0, 0,
     UMI_STATUS_NOT_IMPLEMENTED},
    {"command", "[{\"label\":\"x\",\"command\":{}}]", "a", NULL, 1, 0, 1, 0, 0, UMI_STATUS_NOT_IMPLEMENTED},
    {"owned", "[{\"label\":\"puts\"}]", "a", "puts", 1, 0, 1, 4, 1, UMI_STATUS_OK},
    {"cancel", "[{\"label\":\"puts\"}]", "a", NULL, 1, 0, 1, 0, 0, UMI_STATUS_CANCELLED},
    {"nul", "[{\"label\":\"puts\"}]", "a", NULL, 1, 0, 1, 0, 0, UMI_STATUS_INVALID_ARGUMENT},
    {"limit", "[{\"label\":\"puts\"}]", "a", NULL, 1, 0, 1, 0, 0, UMI_STATUS_CAPACITY_EXCEEDED},
    {"missing-choice", "[]", "a", NULL, 1, 0, 1, 0, 0, UMI_STATUS_NOT_FOUND},
    {"arguments", "[{\"label\":\"puts\"}]", "a", "puts", 1, 0, 1, 4, 1, UMI_STATUS_OK},
};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const PreviewCase *item = NULL;
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(argv[1], cases[i].name) == 0)
            item = &cases[i];
    CHECK(item != NULL);
    UmiLanguageCompletionCatalogue *catalogue = NULL;
    CHECK(UmiLanguageCompletionCatalogueCreate(item->json, strlen(item->json), NULL, &catalogue) ==
          UMI_STATUS_OK);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(item->name, "cancel") == 0)
        umi_cancellation_token_request(cancel);
    size_t bytes = strlen(item->source);
    char *source = malloc(bytes + 1U);
    CHECK(source != NULL);
    memcpy(source, item->source, bytes + 1U);
    if (strcmp(item->name, "nul") == 0)
        ++bytes;
    if (strcmp(item->name, "limit") == 0)
        bytes = 16U * 1024U * 1024U + 1U;
    UmiLanguageCompletionPreview *preview = NULL;
    CHECK(UmiLanguageCompletionPreviewCreate(
              catalogue, 0, "file:///example.c", "selected-language-server", source, bytes, item->caret,
              item->begin, item->end, UMI_LANGUAGE_COMPLETION_REPLACE, cancel, &preview) == item->status);
    CHECK(strcmp(source, item->source) == 0);
    free(source);
    UmiLanguageCompletionCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    if (item->status == UMI_STATUS_OK)
    {
        const char *text = NULL;
        size_t size = 0, caret = 0;
        CHECK(UmiLanguageCompletionPreviewRead(preview, &text, &size, &caret) == UMI_STATUS_OK);
        CHECK(size == strlen(item->result) && strcmp(text, item->result) == 0 && caret == item->result_caret);
        CHECK(UmiLanguageCompletionPreviewEditCount(preview) == item->edits);
        if (strcmp(item->name, "arguments") == 0)
        {
            CHECK(UmiLanguageCompletionPreviewRead(NULL, &text, &size, &caret) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(text == NULL && size == 0U && caret == 0U);
            CHECK(UmiLanguageCompletionPreviewEditCount(NULL) == 0U);
        }
    }
    else
        CHECK(preview == NULL);
    UmiLanguageCompletionPreviewDestroy(preview);
    return 0;
}
