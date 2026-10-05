/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_code_action_catalogue.c
 * PURPOSE: Exercise complete action ownership, metadata validation and whole-operation edit gates.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/code_action_catalogue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                          \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
typedef struct Case
{
    const char *name, *json;
    UmiStatus create, edits;
} Case;
static const Case cases[] = {
    {"basic",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}}]",
     UMI_STATUS_OK, UMI_STATUS_OK},
    {"null", "null", UMI_STATUS_OK, UMI_STATUS_NOT_FOUND},
    {"empty", "[]", UMI_STATUS_OK, UMI_STATUS_NOT_FOUND},
    {"command",
     "[{\"title\":\"Server "
     "action\",\"command\":\"server.run\",\"arguments\":[{\"private\":\"retained\"},3]}]",
     UMI_STATUS_OK, UMI_STATUS_NOT_IMPLEMENTED},
    {"nested-command",
     "[{\"title\":\"Action\",\"command\":{\"title\":\"Server "
     "action\",\"command\":\"server.run\",\"arguments\":[{\"private\":\"retained\"},3]}}]",
     UMI_STATUS_OK, UMI_STATUS_NOT_IMPLEMENTED},
    {"edit-and-command",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}},\"command\":{\"title\":\"Server "
     "action\",\"command\":\"server.run\",\"arguments\":[{\"private\":\"retained\"},3]}}]",
     UMI_STATUS_OK, UMI_STATUS_NOT_IMPLEMENTED},
    {"disabled",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}},\"disabled\":{\"reason\":\"Selection is not suitable\"}}]",
     UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED},
    {"disabled-empty-reason",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}},\"disabled\":{\"reason\":\"\"}}]",
     UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED},
    {"deferred", "[{\"title\":\"Compute edit later\",\"data\":{\"token\":\"saved\"}}]", UMI_STATUS_OK,
     UMI_STATUS_NOT_IMPLEMENTED},
    {"title-only", "[{\"title\":\"Unresolved\"}]", UMI_STATUS_OK, UMI_STATUS_NOT_IMPLEMENTED},
    {"unknown-kind",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"future.custom.kind\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}}]",
     UMI_STATUS_OK, UMI_STATUS_OK},
    {"empty-kind",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}}]",
     UMI_STATUS_OK, UMI_STATUS_OK},
    {"unicode",
     "[{\"title\":\"Rename caf\\u00e9 "
     "\\ud83d\\ude00\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}}]",
     UMI_STATUS_OK, UMI_STATUS_OK},
    {"literal-title",
     "[{\"title\":\"<script>literal</"
     "script>\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}}]",
     UMI_STATUS_OK, UMI_STATUS_OK},
    {"opaque-fields",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}},\"diagnostics\":[{\"message\":\"kept\",\"data\":{\"private\":\"value\"}}],"
     "\"data\":{\"resolve\":\"not executed\"},\"extension\":{\"future\":true}}]",
     UMI_STATUS_OK, UMI_STATUS_OK},
    {"resource",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"documentChanges\":[{\"kind\":"
     "\"create\",\"uri\":\"file:///main.c\"}]}}]",
     UMI_STATUS_OK, UMI_STATUS_NOT_IMPLEMENTED},
    {"invalid-edit",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":7}}]",
     UMI_STATUS_OK, UMI_STATUS_PARSE_ERROR},
    {"annotation",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"documentChanges\":[{"
     "\"textDocument\":{\"uri\":\"file:///"
     "main.c\",\"version\":1},\"edits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{"
     "\"line\":0,\"character\":3}},\"newText\":\"total\",\"annotationId\":\"reason\"}]}],"
     "\"changeAnnotations\":{\"reason\":{\"label\":\"Review edit\",\"needsConfirmation\":true}}}}]",
     UMI_STATUS_OK, UMI_STATUS_OK},
    {"late-invalid",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}},{\"title\":7}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"root-object", "{}", UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"item-null", "[null]", UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"missing-title",
     "[{\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"empty-title", "[{\"title\":\"\"}]", UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"kind-type",
     "[{\"title\":\"Rename "
     "local\",\"kind\":null,\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"preferred-type",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":1,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"disabled-type",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}},\"disabled\":true}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"disabled-missing-reason",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}},\"disabled\":{}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"reason-type",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}},\"disabled\":{\"reason\":7}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"edit-type",
     "[{\"title\":\"Rename local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":null}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"diagnostics-type",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}},\"diagnostics\":{}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"command-null",
     "[{\"title\":\"Rename "
     "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}},\"command\":null}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"command-missing-title", "[{\"title\":\"Action\",\"command\":{\"command\":\"run\"}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"command-empty",
     "[{\"title\":\"Server action\",\"command\":\"\",\"arguments\":[{\"private\":\"retained\"},3]}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"arguments-type", "[{\"title\":\"Server action\",\"command\":\"server.run\",\"arguments\":{}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"mixed-command-edit",
     "[{\"title\":\"Server "
     "action\",\"command\":\"server.run\",\"arguments\":[{\"private\":\"retained\"},3],\"edit\":{\"changes\":"
     "{\"file:///"
     "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":3}},"
     "\"newText\":\"total\"}]}}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
    {"mixed-command-disabled",
     "[{\"title\":\"Server "
     "action\",\"command\":\"server.run\",\"arguments\":[{\"private\":\"retained\"},3],\"disabled\":{"
     "\"reason\":\"blocked\"}}]",
     UMI_STATUS_PARSE_ERROR, UMI_STATUS_NOT_FOUND},
};
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *extra[] = {
        "duplicate-title", "duplicate-edit", "duplicate-command", "invalid-utf8", "invalid-surrogate",
        "owned",           "response",       "wrong-id",          "error",        "cancelled",
        "arguments",       "count-limit",    "count-boundary",    "title-limit",  "kind-limit",
        "reason-limit",    "command-limit",  "selected-good",     "preview"};

    int found = 0;
    const char *json = cases[0].json;
    UmiStatus wanted = UMI_STATUS_OK, edit_wanted = UMI_STATUS_OK;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i].name) == 0)
        {
            found = 1;
            json = cases[i].json;
            wanted = cases[i].create;
            edit_wanted = cases[i].edits;
        }
    for (size_t i = 0U; i < sizeof(extra) / sizeof(extra[0]); ++i)
        if (strcmp(mode, extra[i]) == 0)
            found = 1;
    CHECK(found);
    if (strcmp(mode, "duplicate-title") == 0)
    {
        json = "[{\"title\":\"a\",\"title\":\"b\"}]";
        wanted = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "duplicate-edit") == 0)
    {
        json = "[{\"title\":\"a\",\"edit\":{},\"edit\":{}}]";
        wanted = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "duplicate-command") == 0)
    {
        json = "[{\"title\":\"a\",\"command\":{\"title\":\"b\",\"command\":\"c\",\"command\":\"d\"}}]";
        wanted = UMI_STATUS_ALREADY_EXISTS;
    }
    if (strcmp(mode, "invalid-utf8") == 0)
    {
        json = "[{\"title\":\"\xc0\x80\"}]";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "invalid-surrogate") == 0)
    {
        json = "[{\"title\":\"\\ud800\"}]";
        wanted = UMI_STATUS_PARSE_ERROR;
    }
    char *allocated = NULL;
    if (strcmp(mode, "count-limit") == 0 || strcmp(mode, "count-boundary") == 0)
    {
        size_t count = strcmp(mode, "count-limit") == 0 ? 513U : 512U;
        allocated = calloc(20000U, 1U);
        CHECK(allocated != NULL);
        strcpy(allocated, "[");
        for (size_t i = 0U; i < count; ++i)
            strcat(allocated, i == 0U ? "{\"title\":\"Action\"}" : ",{\"title\":\"Action\"}");
        strcat(allocated, "]");
        json = allocated;
        if (count == 513U)
            wanted = UMI_STATUS_CAPACITY_EXCEEDED;
        edit_wanted = UMI_STATUS_NOT_IMPLEMENTED;
    }
    if (strcmp(mode, "title-limit") == 0 || strcmp(mode, "kind-limit") == 0 ||
        strcmp(mode, "reason-limit") == 0 || strcmp(mode, "command-limit") == 0)
    {
        const char *prefix = strcmp(mode, "title-limit") == 0  ? "[{\"title\":\""
                             : strcmp(mode, "kind-limit") == 0 ? "[{\"title\":\"A\",\"kind\":\""
                             : strcmp(mode, "reason-limit") == 0
                                 ? "[{\"title\":\"A\",\"disabled\":{\"reason\":\""
                                 : "[{\"title\":\"A\",\"command\":\"";
        const char *suffix = strcmp(mode, "reason-limit") == 0 ? "\"}}]" : "\"}]";
        size_t size =
            (strcmp(mode, "title-limit") == 0 || strcmp(mode, "reason-limit") == 0) ? 65537U : 4097U;
        size_t length = strlen(prefix) + size + strlen(suffix);
        allocated = malloc(length + 1U);
        CHECK(allocated != NULL);
        memcpy(allocated, prefix, strlen(prefix));
        memset(allocated + strlen(prefix), 'a', size);
        strcpy(allocated + strlen(prefix) + size, suffix);
        json = allocated;
        wanted = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (strcmp(mode, "owned") == 0)
    {
        allocated = malloc(strlen(json) + 1U);
        CHECK(allocated != NULL);
        strcpy(allocated, json);
        json = allocated;
    }
    if (strcmp(mode, "selected-good") == 0)
        json = "[{\"title\":\"Rename "
               "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"documentChanges\":[{"
               "\"kind\":\"delete\",\"uri\":\"file:///other.c\"}]}},{\"title\":\"Rename "
               "local\",\"kind\":\"refactor.rename\",\"isPreferred\":true,\"edit\":{\"changes\":{\"file:///"
               "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,"
               "\"character\":3}},\"newText\":\"total\"}]}}}]";

    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        wanted = UMI_STATUS_CANCELLED;
    }
    UmiLanguageCodeActionCatalogue *catalogue = NULL;
    UmiStatus status;
    if (strcmp(mode, "response") == 0 || strcmp(mode, "wrong-id") == 0 || strcmp(mode, "error") == 0)
    {
        char response[4096];
        if (strcmp(mode, "error") == 0)
        {
            strcpy(response,
                   "{\"jsonrpc\":\"2.0\",\"id\":1,\"error\":{\"code\":-1,\"message\":\"refused\"}}");
            wanted = UMI_STATUS_UNAVAILABLE;
        }
        else
            (void)snprintf(response, sizeof(response), "{\"jsonrpc\":\"2.0\",\"id\":1,\"result\":%s}", json);
        uint64_t id = strcmp(mode, "wrong-id") == 0 ? 2U : 1U;
        if (id == 2U)
            wanted = UMI_STATUS_NOT_FOUND;
        status =
            UmiLanguageCodeActionCatalogueReadResponse(response, strlen(response), id, cancel, &catalogue);
    }
    else
        status = UmiLanguageCodeActionCatalogueCreate(json, strlen(json), cancel, &catalogue);
    if (status != wanted)
        fprintf(stderr, "%s: status %d wanted %d\n", mode, (int)status, (int)wanted);
    CHECK(status == wanted);
    if (strcmp(mode, "owned") == 0)
        memset(allocated, 'x', strlen(allocated));
    if (status == UMI_STATUS_OK)
    {
        size_t count = UmiLanguageCodeActionCatalogueCount(catalogue);
        if (strcmp(mode, "null") == 0 || strcmp(mode, "empty") == 0)
            CHECK(count == 0U);
        else if (strcmp(mode, "count-boundary") == 0)
            CHECK(count == 512U);
        else
            CHECK(count == (strcmp(mode, "selected-good") == 0 ? 2U : 1U));
        UmiLanguageCodeAction action;
        memset(&action, 0, sizeof(action));
        action.title = "sentinel";
        CHECK(UmiLanguageCodeActionCatalogueAt(catalogue, count, &action) == UMI_STATUS_NOT_FOUND &&
              strcmp(action.title, "sentinel") == 0);
        if (count != 0U)
        {
            size_t selected = strcmp(mode, "selected-good") == 0 ? 1U : 0U;
            CHECK(UmiLanguageCodeActionCatalogueAt(catalogue, selected, &action) == UMI_STATUS_OK);
            CHECK(action.title != NULL && action.kind != NULL && action.disabled_reason != NULL &&
                  action.command != NULL);
            if (strcmp(mode, "basic") == 0 || strcmp(mode, "owned") == 0)
                CHECK(strcmp(action.title, "Rename local") == 0 && action.preferred && action.has_edit &&
                      !action.has_command);
            if (strcmp(mode, "unknown-kind") == 0)
                CHECK(strcmp(action.kind, "future.custom.kind") == 0);
            if (strcmp(mode, "disabled") == 0)
                CHECK(action.disabled && strcmp(action.disabled_reason, "Selection is not suitable") == 0);
            if (strcmp(mode, "command") == 0 || strcmp(mode, "nested-command") == 0 ||
                strcmp(mode, "edit-and-command") == 0)
                CHECK(action.has_command && strcmp(action.command, "server.run") == 0);
            const char *raw = NULL;
            size_t bytes = 0U;
            CHECK(UmiLanguageCodeActionCatalogueItemJson(catalogue, selected, &raw, &bytes) ==
                      UMI_STATUS_OK &&
                  bytes > 2U && raw[0] == '{');
            if (strcmp(mode, "opaque-fields") == 0 || strcmp(mode, "command") == 0)
            {
                char *copy = malloc(bytes + 1U);
                CHECK(copy != NULL);
                memcpy(copy, raw, bytes);
                copy[bytes] = '\0';
                CHECK(strstr(copy, "private") != NULL);
                free(copy);
            }
            UmiLanguageWorkspaceEditCatalogue *edits = NULL;
            CHECK(UmiLanguageCodeActionCatalogueReadEdits(catalogue, selected, cancel, &edits) ==
                  edit_wanted);
            if (edit_wanted == UMI_STATUS_OK)
            {
                CHECK(UmiLanguageWorkspaceEditCatalogueCount(edits) == 1U);
                if (strcmp(mode, "annotation") == 0)
                {
                    UmiLanguageWorkspaceChangeAnnotation annotation;
                    CHECK(UmiLanguageWorkspaceEditCatalogueAnnotation(edits, 0U, &annotation) ==
                              UMI_STATUS_OK &&
                          annotation.needs_confirmation);
                }
                if (strcmp(mode, "preview") == 0)
                {
                    UmiLanguageTextEditPreview *preview = NULL;
                    int32_t version = 1;
                    const char *text = NULL;
                    size_t size = 0U, caret = 0U;
                    CHECK(UmiLanguageWorkspaceEditCataloguePreviewSingleDocument(
                              edits, "file:///main.c", "sum", 3U, 1U, &version, NULL, &preview) ==
                          UMI_STATUS_OK);
                    CHECK(UmiLanguageTextEditPreviewRead(preview, &text, &size, &caret) == UMI_STATUS_OK &&
                          size == 5U && strcmp(text, "total") == 0);
                    UmiLanguageTextEditPreviewDestroy(preview);
                }
            }
            else
                CHECK(edits == NULL);
            UmiLanguageWorkspaceEditCatalogueDestroy(edits);
        }
        if (strcmp(mode, "arguments") == 0)
        {
            const char *raw = (const char *)1;
            size_t bytes = 123U;
            UmiLanguageWorkspaceEditCatalogue *edits = NULL;
            CHECK(UmiLanguageCodeActionCatalogueAt(NULL, 0U, &action) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageCodeActionCatalogueAt(catalogue, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageCodeActionCatalogueItemJson(catalogue, 99U, &raw, &bytes) ==
                      UMI_STATUS_NOT_FOUND &&
                  raw == NULL && bytes == 0U);
            CHECK(UmiLanguageCodeActionCatalogueReadEdits(catalogue, 99U, NULL, &edits) ==
                      UMI_STATUS_NOT_FOUND &&
                  edits == NULL);
            CHECK(UmiLanguageCodeActionCatalogueReadEdits(catalogue, 0U, NULL, NULL) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiLanguageCodeActionCatalogueCount(NULL) == 0U);
            UmiLanguageCodeActionCatalogueDestroy(NULL);
        }
    }
    else
        CHECK(catalogue == NULL);
    UmiLanguageCodeActionCatalogueDestroy(catalogue);
    umi_cancellation_token_destroy(cancel);
    free(allocated);
    return 0;
}
