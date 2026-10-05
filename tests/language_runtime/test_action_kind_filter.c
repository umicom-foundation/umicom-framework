/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/language_runtime/test_action_kind_filter.c
 * PURPOSE: Check independent action-family catalogues, kind boundaries and whole-proposal metadata.
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
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
#define EDIT                                                                                                 \
    "\"edit\":{\"changes\":{\"file:///"                                                                      \
    "main.c\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},\"end\":{\"line\":0,\"character\":1}},"    \
    "\"newText\":\"B\"}]}}"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"all",         "quickfix",      "refactor",        "organize",
                           "fix-all",     "descendants",   "prefix-boundary", "missing-kind",
                           "empty-kind",  "unrelated",     "empty",           "null",
                           "independent", "original-json", "disabled",        "command",
                           "deferred",    "preferred",     "invalid-filter",  "null-source",
                           "null-output", "name-invalid",  "name-null",       "cancelled",
                           "capacity",    "order"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    const char *json = "[{\"title\":\"quick\",\"kind\":\"quickfix\"},{\"title\":\"extract\",\"kind\":"
                       "\"refactor.extract\"},{\"title\":\"imports\",\"kind\":\"source.organizeImports\"},{"
                       "\"title\":\"fixes\",\"kind\":\"source.fixAll\"},{\"title\":\"unclassified\"}]";
    UmiLanguageCodeActionFilter filter = UMI_LANGUAGE_ACTION_FILTER_REFACTOR;
    size_t expected_count = 1U;
    const char *expected_title = "extract";
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "all") == 0)
    {
        filter = UMI_LANGUAGE_ACTION_FILTER_ALL;
        expected_count = 5U;
        expected_title = "quick";
    }
    if (strcmp(mode, "quickfix") == 0)
    {
        filter = UMI_LANGUAGE_ACTION_FILTER_QUICK_FIX;
        expected_title = "quick";
    }
    if (strcmp(mode, "organize") == 0)
    {
        filter = UMI_LANGUAGE_ACTION_FILTER_ORGANIZE_IMPORTS;
        expected_title = "imports";
    }
    if (strcmp(mode, "fix-all") == 0)
    {
        filter = UMI_LANGUAGE_ACTION_FILTER_FIX_ALL;
        expected_title = "fixes";
    }
    if (strcmp(mode, "descendants") == 0)
    {
        json = "[{\"title\":\"extract\",\"kind\":\"refactor.extract.function\"}]";
    }
    if (strcmp(mode, "prefix-boundary") == 0)
    {
        json = "[{\"title\":\"wrong\",\"kind\":\"refactorOther\"},{\"title\":\"extract\",\"kind\":\"refactor."
               "extract\"}]";
    }
    if (strcmp(mode, "missing-kind") == 0)
    {
        json = "[{\"title\":\"unclassified\"}]";
        expected_count = 0U;
    }
    if (strcmp(mode, "empty-kind") == 0)
    {
        json = "[{\"title\":\"unclassified\",\"kind\":\"\"}]";
        expected_count = 0U;
    }
    if (strcmp(mode, "unrelated") == 0)
    {
        json = "[{\"title\":\"other\",\"kind\":\"source.fixAll\"}]";
        expected_count = 0U;
    }
    if (strcmp(mode, "empty") == 0)
    {
        json = "[]";
        expected_count = 0U;
    }
    if (strcmp(mode, "null") == 0)
    {
        json = "null";
        expected_count = 0U;
    }
    if (strcmp(mode, "original-json") == 0)
        json = "[{\"title\":\"extract\",\"kind\":\"refactor.extract\",\"data\": "
               "{\"opaque\":[1,\"value\"]},\"future\":true," EDIT "}]";
    if (strcmp(mode, "disabled") == 0)
        json = "[{\"title\":\"extract\",\"kind\":\"refactor.extract\",\"disabled\":{\"reason\":\"Not "
               "here\"}," EDIT "}]";
    if (strcmp(mode, "command") == 0)
        json = "[{\"title\":\"extract\",\"kind\":\"refactor.extract\",\"command\":{\"title\":\"Required "
               "command\",\"command\":\"server.run\",\"arguments\":[3]}," EDIT "}]";
    if (strcmp(mode, "deferred") == 0)
        json = "[{\"title\":\"extract\",\"kind\":\"refactor.extract\",\"data\":{\"opaque\":3}}]";
    if (strcmp(mode, "preferred") == 0)
        json = "[{\"title\":\"extract\",\"kind\":\"refactor.extract\",\"isPreferred\":true," EDIT "}]";
    if (strcmp(mode, "invalid-filter") == 0)
    {
        filter = (UmiLanguageCodeActionFilter)99;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    char *many = NULL;
    if (strcmp(mode, "capacity") == 0 || strcmp(mode, "order") == 0)
    {
        many = malloc(65536U);
        CHECK(many != NULL);
        size_t used = 1U;
        many[0] = '[';
        for (size_t i = 0U; i < 512U; ++i)
        {
            int n = snprintf(many + used, 65536U - used, "%s{\"title\":\"row-%zu\",\"kind\":\"%s\"}",
                             i == 0U ? "" : ",", i, i % 2U == 0U ? "refactor.extract" : "quickfix");
            CHECK(n > 0 && (size_t)n < 65536U - used);
            used += (size_t)n;
        }
        many[used++] = ']';
        many[used] = '\0';
        json = many;
        expected_count = 256U;
        expected_title = "row-0";
    }
    UmiLanguageCodeActionCatalogue *source = NULL, *selected = NULL;
    CHECK(UmiLanguageCodeActionCatalogueCreate(json, strlen(json), NULL, &source) == UMI_STATUS_OK);
    const size_t original_count = UmiLanguageCodeActionCatalogueCount(source);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    const char *kind = NULL;
    if (strcmp(mode, "name-invalid") == 0)
    {
        CHECK(UmiLanguageCodeActionFilterName((UmiLanguageCodeActionFilter)99, &kind) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              kind == NULL);
        goto done;
    }
    if (strcmp(mode, "name-null") == 0)
    {
        CHECK(UmiLanguageCodeActionFilterName(filter, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if (strcmp(mode, "null-output") == 0)
    {
        CHECK(UmiLanguageCodeActionCatalogueFilter(source, filter, cancel, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
        goto done;
    }
    if (strcmp(mode, "null-source") == 0)
    {
        CHECK(UmiLanguageCodeActionCatalogueFilter(NULL, filter, cancel, &selected) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              selected == NULL);
        goto done;
    }
    CHECK(UmiLanguageCodeActionCatalogueFilter(source, filter, cancel, &selected) == expected);
    CHECK(UmiLanguageCodeActionCatalogueCount(source) == original_count);
    if (expected != UMI_STATUS_OK)
    {
        CHECK(selected == NULL);
        goto done;
    }
    CHECK(UmiLanguageCodeActionCatalogueCount(selected) == expected_count);
    if (strcmp(mode, "original-json") == 0)
    {
        const char *original = NULL, *copy = NULL;
        size_t first = 0U, second = 0U;
        CHECK(UmiLanguageCodeActionCatalogueItemJson(source, 0U, &original, &first) == UMI_STATUS_OK);
        CHECK(UmiLanguageCodeActionCatalogueItemJson(selected, 0U, &copy, &second) == UMI_STATUS_OK);
        CHECK(first == second && memcmp(original, copy, first) == 0 && original != copy);
    }
    if (strcmp(mode, "independent") == 0)
    {
        UmiLanguageCodeActionCatalogueDestroy(source);
        source = NULL;
    }
    if (expected_count != 0U)
    {
        UmiLanguageCodeAction action;
        CHECK(UmiLanguageCodeActionCatalogueAt(selected, 0U, &action) == UMI_STATUS_OK);
        CHECK(strcmp(action.title, expected_title) == 0);
        if (strcmp(mode, "disabled") == 0)
            CHECK(action.disabled && strcmp(action.disabled_reason, "Not here") == 0);
        if (strcmp(mode, "preferred") == 0)
            CHECK(action.preferred);
        if (strcmp(mode, "command") == 0)
            CHECK(action.has_command && action.has_edit && strcmp(action.command, "server.run") == 0);
        if (strcmp(mode, "deferred") == 0 || strcmp(mode, "original-json") == 0)
            CHECK(action.has_data);
        if (strcmp(mode, "disabled") == 0 || strcmp(mode, "command") == 0 || strcmp(mode, "deferred") == 0 ||
            strcmp(mode, "preferred") == 0 || strcmp(mode, "original-json") == 0)
        {
            UmiLanguageWorkspaceEditCatalogue *edits = NULL;
            UmiStatus edit_status = strcmp(mode, "disabled") == 0 ? UMI_STATUS_PERMISSION_DENIED
                                    : strcmp(mode, "command") == 0 || strcmp(mode, "deferred") == 0
                                        ? UMI_STATUS_NOT_IMPLEMENTED
                                        : UMI_STATUS_OK;
            CHECK(UmiLanguageCodeActionCatalogueReadEdits(selected, 0U, NULL, &edits) == edit_status);
            if (edit_status != UMI_STATUS_OK)
                CHECK(edits == NULL);
            else
                CHECK(UmiLanguageWorkspaceEditCatalogueCount(edits) == 1U);
            UmiLanguageWorkspaceEditCatalogueDestroy(edits);
        }
    }
    if (strcmp(mode, "order") == 0)
        for (size_t i = 0U; i < expected_count; ++i)
        {
            UmiLanguageCodeAction action;
            char title[40];
            (void)snprintf(title, sizeof(title), "row-%zu", i * 2U);
            CHECK(UmiLanguageCodeActionCatalogueAt(selected, i, &action) == UMI_STATUS_OK &&
                  strcmp(action.title, title) == 0);
        }
done:
    UmiLanguageCodeActionCatalogueDestroy(selected);
    UmiLanguageCodeActionCatalogueDestroy(source);
    umi_cancellation_token_destroy(cancel);
    free(many);
    return 0;
}
