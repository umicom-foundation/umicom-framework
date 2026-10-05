/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/editor/test_snippet_storage.c
 * PURPOSE: Verify strict named-template storage, bounded metadata, safe path identities and complete local replacement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/developer_project/snippet_storage.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *cases[] = {"round-trip",        "unicode",         "escaped",
                           "empty-body",        "body-limit",      "encode-capacity",
                           "unterminated",      "invalid-unicode", "structure",
                           "language-empty",    "bytes-limit",     "output-arguments",
                           "path-case",         "path-language",   "path-separator",
                           "path-capacity",     "path-id-limit",   "path-language-limit",
                           "path-invalid-base", "save-load",       "replace",
                           "invalid-save",      "wrong-id",        "wrong-language",
                           "missing-file",      "json-array",      "json-null",
                           "missing",           "unknown",         "duplicate",
                           "wrong-type",        "wrong-format",    "empty-id",
                           "blank-name",        "control-id",      "nul-body",
                           "bad-surrogate",     "trailing"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    UmiEditorSnippetTemplate *snippet = calloc(1U, sizeof(*snippet)), *decoded = malloc(sizeof(*decoded)),
                             *sentinel = malloc(sizeof(*sentinel));
    CHECK(snippet && decoded && sentinel);
    memset(sentinel, 0x5a, sizeof(*sentinel));
    *decoded = *sentinel;
    snippet->struct_size = (uint32_t)sizeof(*snippet);
    snippet->api_version = UMI_EDITOR_SNIPPET_SESSION_API_VERSION;
    strcpy(snippet->id, "loop");
    strcpy(snippet->language_id, "c");
    strcpy(snippet->name, "Loop");
    strcpy(snippet->body, "${1:value}-$1$0");
    char *encoded = malloc(65536U);
    CHECK(encoded != NULL);
    size_t bytes = 0U;
    if (strcmp(mode, "unicode") == 0)
    {
        strcpy(snippet->id, "caf\xc3\xa9");
        strcpy(snippet->name, "Caf\xc3\xa9");
        strcpy(snippet->body, "${1:\xf0\x9f\x98\x80}$0");
    }
    if (strcmp(mode, "escaped") == 0)
        strcpy(snippet->body, "one\r\ntwo\t\"three\"\\end");
    if (strcmp(mode, "empty-body") == 0)
        snippet->body[0] = '\0';
    if (strcmp(mode, "body-limit") == 0)
    {
        memset(snippet->body, '\t', sizeof(snippet->body) - 1U);
        snippet->body[sizeof(snippet->body) - 1U] = '\0';
    }
    CHECK(UmiSnippetTemplateEncode(snippet, encoded, 65536U, &bytes) == UMI_STATUS_OK);
    CHECK(UmiSnippetTemplateDecode(encoded, bytes, decoded) == UMI_STATUS_OK &&
          memcmp(decoded, snippet, sizeof(*snippet)) == 0);
    *decoded = *sentinel;
    const char *bad = NULL;
    /* Each malformed record reaches the decoder with its original bytes.
     * Keep these examples explicit so new storage fields receive rejection coverage. */
    if (strcmp(mode, "json-array") == 0)
        bad = "[]";
    if (strcmp(mode, "json-null") == 0)
        bad = "null";
    if (strcmp(mode, "missing") == 0)
        bad = "{}";
    if (strcmp(mode, "unknown") == 0)
        bad = "{\"format\":\"umicom.local-snippet\",\"id\":\"loop\",\"language\":\"c\",\"name\":\"Loop\","
              "\"body\":\"x\",\"extra\":0}";
    if (strcmp(mode, "duplicate") == 0)
        bad = "{\"format\":\"umicom.local-snippet\",\"id\":\"loop\",\"id\":\"other\",\"language\":\"c\","
              "\"name\":\"Loop\"}";
    if (strcmp(mode, "wrong-type") == 0)
        bad = "{\"format\":\"umicom.local-snippet\",\"id\":\"loop\",\"language\":\"c\",\"name\":\"Loop\","
              "\"body\":42}";
    if (strcmp(mode, "wrong-format") == 0)
        bad = "{\"format\":\"other\",\"id\":\"loop\",\"language\":\"c\",\"name\":\"Loop\",\"body\":\"x\"}";
    if (strcmp(mode, "empty-id") == 0)
        bad = "{\"format\":\"umicom.local-snippet\",\"id\":\"\",\"language\":\"c\",\"name\":\"Loop\","
              "\"body\":\"x\"}";
    if (strcmp(mode, "blank-name") == 0)
        bad = "{\"format\":\"umicom.local-snippet\",\"id\":\"loop\",\"language\":\"c\",\"name\":\"  "
              "\",\"body\":\"x\"}";
    if (strcmp(mode, "control-id") == 0)
        bad = "{\"format\":\"umicom.local-snippet\",\"id\":\"lo\\nop\",\"language\":\"c\",\"name\":\"Loop\","
              "\"body\":\"x\"}";
    if (strcmp(mode, "nul-body") == 0)
        bad = "{\"format\":\"umicom.local-snippet\",\"id\":\"loop\",\"language\":\"c\",\"name\":\"Loop\","
              "\"body\":\"\\u0000\"}";
    if (strcmp(mode, "bad-surrogate") == 0)
        bad = "{\"format\":\"umicom.local-snippet\",\"id\":\"loop\",\"language\":\"c\",\"name\":\"Loop\","
              "\"body\":\"\\ud800\"}";
    if (strcmp(mode, "trailing") == 0)
        bad = "{\"format\":\"umicom.local-snippet\",\"id\":\"loop\",\"language\":\"c\",\"name\":\"Loop\","
              "\"body\":\"x\"} false";
    if (bad != NULL)
    {
        CHECK(UmiSnippetTemplateDecode(bad, strlen(bad), decoded) != UMI_STATUS_OK);
        CHECK(memcmp(decoded, sentinel, sizeof(*decoded)) == 0);
    }
    else if (strcmp(mode, "encode-capacity") == 0)
    {
        char short_output[8] = "kept";
        size_t size = 99U;
        CHECK(UmiSnippetTemplateEncode(snippet, short_output, sizeof(short_output), &size) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(short_output, "kept") == 0 && size == 99U);
    }
    else if (strcmp(mode, "unterminated") == 0 || strcmp(mode, "invalid-unicode") == 0 ||
             strcmp(mode, "structure") == 0 || strcmp(mode, "language-empty") == 0)
    {
        if (strcmp(mode, "unterminated") == 0)
            memset(snippet->name, 'x', sizeof(snippet->name));
        if (strcmp(mode, "invalid-unicode") == 0)
            strcpy(snippet->body, "\xff");
        if (strcmp(mode, "structure") == 0)
            snippet->struct_size = 0U;
        if (strcmp(mode, "language-empty") == 0)
            snippet->language_id[0] = '\0';
        CHECK(UmiSnippetTemplateValidate(snippet) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(mode, "bytes-limit") == 0)
    {
        char *large = malloc(65537U);
        CHECK(large != NULL);
        memset(large, ' ', 65537U);
        CHECK(UmiSnippetTemplateDecode(large, 65537U, decoded) == UMI_STATUS_CAPACITY_EXCEEDED);
        free(large);
        CHECK(memcmp(decoded, sentinel, sizeof(*decoded)) == 0);
    }
    else if (strcmp(mode, "output-arguments") == 0)
    {
        CHECK(UmiSnippetTemplateDecode(NULL, 0U, decoded) == UMI_STATUS_INVALID_ARGUMENT &&
              memcmp(decoded, sentinel, sizeof(*decoded)) == 0);
        CHECK(UmiSnippetTemplateEncode(snippet, NULL, 65536U, &bytes) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strncmp(mode, "path-", 5U) == 0)
    {
        char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY];
        FixtureDirectory(root);
        CHECK(UmiSnippetTemplatePath("Studio", root, "c", "loop", path, sizeof(path)) == UMI_STATUS_OK &&
              umi_path_is_within(root, path));
        if (strcmp(mode, "path-case") == 0 || strcmp(mode, "path-language") == 0 ||
            strcmp(mode, "path-separator") == 0)
        {
            const char *language = strcmp(mode, "path-language") == 0 ? "C" : "c";
            const char *id = strcmp(mode, "path-separator") == 0 ? "../loop" : "Loop";
            CHECK(UmiSnippetTemplatePath("Studio", root, language, id, other, sizeof(other)) ==
                      UMI_STATUS_OK &&
                  umi_path_is_within(root, other) && strcmp(path, other) != 0);
        }
        else if (strcmp(mode, "path-capacity") == 0 || strcmp(mode, "path-invalid-base") == 0)
        {
            strcpy(other, "kept");
            CHECK(UmiSnippetTemplatePath(
                      "Studio", strcmp(mode, "path-invalid-base") == 0 ? "relative" : root, "c", "loop",
                      other, strcmp(mode, "path-capacity") == 0 ? 3U : sizeof(other)) != UMI_STATUS_OK &&
                  strcmp(other, "kept") == 0);
        }
        else
        {
            char name[129];
            size_t maximum = strcmp(mode, "path-id-limit") == 0 ? 127U : 63U;
            memset(name, 'x', maximum);
            name[maximum] = '\0';
            CHECK(UmiSnippetTemplatePath("Studio", root, maximum == 63U ? name : "c",
                                         maximum == 127U ? name : "loop", other,
                                         sizeof(other)) == UMI_STATUS_OK);
            /* Every native component must fit even when both identities are
             * encoded at their maximum supported byte lengths. */
            size_t component = 0U;
            for (size_t i = 0U; i <= strlen(other); ++i)
            {
                if (other[i] == '/' || other[i] == '\\' || other[i] == '\0')
                {
                    CHECK(component <= 255U);
                    component = 0U;
                }
                else
                    ++component;
            }
            name[maximum] = 'x';
            name[maximum + 1U] = '\0';
            CHECK(UmiSnippetTemplatePath("Studio", root, maximum == 63U ? name : "c",
                                         maximum == 127U ? name : "loop", other,
                                         sizeof(other)) == UMI_STATUS_CAPACITY_EXCEEDED);
        }
    }
    else if (strcmp(mode, "save-load") == 0 || strcmp(mode, "replace") == 0 ||
             strcmp(mode, "invalid-save") == 0 || strcmp(mode, "wrong-id") == 0 ||
             strcmp(mode, "wrong-language") == 0 || strcmp(mode, "missing-file") == 0)
    {
        char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
        FixtureDirectory(root);
        FixturePath(path, root, "caf\xc3\xa9.json");
        if (strcmp(mode, "missing-file") == 0)
            CHECK(UmiSnippetTemplateLoad(path, "c", "loop", decoded) != UMI_STATUS_OK);
        else
        {
            CHECK(UmiSnippetTemplateSave(path, snippet) == UMI_STATUS_OK);
            if (strcmp(mode, "replace") == 0)
            {
                strcpy(snippet->body, "${1:new}$0");
                CHECK(UmiSnippetTemplateSave(path, snippet) == UMI_STATUS_OK);
            }
            if (strcmp(mode, "invalid-save") == 0)
            {
                snippet->struct_size = 0U;
                CHECK(UmiSnippetTemplateSave(path, snippet) == UMI_STATUS_INVALID_ARGUMENT);
                snippet->struct_size = (uint32_t)sizeof(*snippet);
            }
            const char *language = strcmp(mode, "wrong-language") == 0 ? "C" : "c",
                       *id = strcmp(mode, "wrong-id") == 0 ? "Loop" : "loop";
            UmiStatus expected = strcmp(mode, "wrong-language") == 0 || strcmp(mode, "wrong-id") == 0
                                     ? UMI_STATUS_INVALID_STATE
                                     : UMI_STATUS_OK;
            CHECK(UmiSnippetTemplateLoad(path, language, id, decoded) == expected);
            CHECK(memcmp(decoded, expected == UMI_STATUS_OK ? snippet : sentinel, sizeof(*decoded)) == 0);
        }
    }
    free(encoded);
    free(snippet);
    free(decoded);
    free(sentinel);
    return 0;
}
