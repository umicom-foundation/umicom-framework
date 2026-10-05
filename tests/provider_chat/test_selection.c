/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_chat/test_selection.c
 * PURPOSE: Verify exact selected-byte capture, immutable output and stale metadata rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/document_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
#define OK(c) CHECK((c) == UMI_STATUS_OK)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"selection", "large",  "immutable", "stale",   "empty",   "inactive",
                           "overflow",  "bounds", "capacity",  "missing", "invalid", "unicode"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    UmiUiDocumentViewModel *model = NULL;
    OK(umi_ui_document_view_model_create(&model));
    UmiUiDocumentViewSnapshot view = {0};
    strcpy(view.view_id, "editor.test");
    strcpy(view.document_id, "document.test");
    strcpy(view.title, "Title must not enter exported text");
    strcpy(view.uri, "file:///private/path/never-sent.c");
    view.active = 1;
    view.dirty = 1;
    view.cursor_offset = 7U;
    view.selection_length = 6U;
    const char *input = "before chosen after";
    char *large = NULL;
    if (strcmp(name, "large") == 0)
    {
        large = malloc(40001U);
        CHECK(large != NULL);
        memset(large, 'x', 40000U);
        large[40000] = '\0';
        memcpy(large + 30000U, "chosen", 6U);
        input = large;
        view.cursor_offset = 30000U;
    }
    if (strcmp(name, "unicode") == 0)
    {
        input = "a caf\xc3\xa9 z";
        view.cursor_offset = 2U;
        view.selection_length = 5U;
    }
    if (strcmp(name, "empty") == 0)
        view.selection_length = 0U;
    if (strcmp(name, "inactive") == 0)
        view.active = 0;
    if (strcmp(name, "overflow") == 0)
        view.cursor_offset = SIZE_MAX;
    if (strcmp(name, "bounds") == 0)
        view.selection_length = SIZE_MAX;
    OK(UmiUiDocumentViewModelUpsertText(model, &view, input, strlen(input)));
    uint64_t revision = umi_ui_document_view_model_revision(model);
    if (strcmp(name, "stale") == 0)
    {
        OK(umi_ui_document_view_model_find(model, "editor.test", &view));
        view.cursor_offset = 0U;
        OK(umi_ui_document_view_model_upsert(model, &view));
    }
    char text[64];
    memset(text, '!', sizeof(text));
    UmiUiDocumentSelectionInfo info, before;
    memset(&info, 0x5a, sizeof(info));
    before = info;
    UmiStatus status = UmiUiDocumentViewModelCopySelection(
        strcmp(name, "invalid") == 0 ? NULL : model, strcmp(name, "missing") == 0 ? "missing" : "editor.test",
        revision, text, strcmp(name, "capacity") == 0 ? 6U : sizeof(text), &info);
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "stale") == 0)
        expected = UMI_STATUS_BUSY;
    if (strcmp(name, "empty") == 0 || strcmp(name, "inactive") == 0)
        expected = UMI_STATUS_INVALID_STATE;
    if (strcmp(name, "overflow") == 0 || strcmp(name, "bounds") == 0 || strcmp(name, "invalid") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(name, "capacity") == 0)
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(name, "missing") == 0)
        expected = UMI_STATUS_NOT_FOUND;
    CHECK(status == expected);
    if (status == UMI_STATUS_OK)
    {
        CHECK(strcmp(text, strcmp(name, "unicode") == 0 ? "caf\xc3\xa9" : "chosen") == 0);
        CHECK(info.byte_count == strlen(text) && info.byte_offset == view.cursor_offset);
        CHECK(strcmp(info.view_id, "editor.test") == 0 && strcmp(info.document_id, "document.test") == 0 &&
              info.text_revision != 0U);
        if (strcmp(name, "immutable") == 0)
        {
            OK(umi_ui_document_view_model_find(model, "editor.test", &view));
            OK(UmiUiDocumentViewModelUpsertText(model, &view, "replacement", 11U));
            CHECK(strcmp(text, "chosen") == 0);
        }
    }
    else
    {
        CHECK(memcmp(&info, &before, sizeof(info)) == 0);
        for (size_t i = 0U; i < sizeof(text); ++i)
            CHECK(text[i] == '!');
    }
    umi_ui_document_view_model_destroy(model);
    free(large);
    return 0;
}
