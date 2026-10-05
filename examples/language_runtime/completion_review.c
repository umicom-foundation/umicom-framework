/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/language_runtime/completion_review.c
 * PURPOSE: Show a completion and its import as one explicitly accepted buffer edit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/completion_catalogue.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc > 2 || (argc == 2 && strcmp(argv[1], "--apply") != 0))
    {
        fprintf(stderr, "Usage: umicom-completion-review-example [--apply]\n");
        return 2;
    }
    /* This small lesson uses a captured protocol result, so no server or files
     * are needed. A product obtains the result from its correlated connection. */
    const char source[] = "int main(void) {\n    pu\n}\n";
    const char result[] =
        "[{\"label\":\"puts\",\"textEdit\":{\"range\":{\"start\":{\"line\":1,\"character\":4},"
        "\"end\":{\"line\":1,\"character\":6}},\"newText\":\"puts(\\\"Hello\\\");\"},"
        "\"additionalTextEdits\":[{\"range\":{\"start\":{\"line\":0,\"character\":0},"
        "\"end\":{\"line\":0,\"character\":0}},\"newText\":\"#include <stdio.h>\\n\"}]}]";
    UmiEditorTextBuffer *buffer = NULL;
    UmiLanguageCompletionCatalogue *catalogue = NULL;
    UmiEditorWorkspaceEditSet *edits = NULL;
    UmiStatus status = umi_editor_text_buffer_create(128U, &buffer);
    if (status == UMI_STATUS_OK)
        status = umi_editor_text_buffer_set(buffer, source, strlen(source));
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCompletionCatalogueCreate(result, strlen(result), NULL, &catalogue);
    if (status == UMI_STATUS_OK)
    {
        UmiLanguageCompletionContext context = {"file:///lesson/main.c",
                                                "lesson",
                                                umi_editor_text_buffer_revision(buffer),
                                                {1U, 6U},
                                                {{1U, 4U}, {1U, 6U}},
                                                UMI_LANGUAGE_COMPLETION_REPLACE};
        status = UmiLanguageCompletionPlanCreate(catalogue, 0U, &context, buffer, NULL, &edits);
    }
    if (status == UMI_STATUS_OK)
    {
        puts("Planned changes (zero-based lines, UTF-16 columns):");
        for (size_t i = 0U; i < umi_editor_workspace_edit_set_count(edits); ++i)
        {
            UmiEditorWorkspaceTextEdit edit;
            status = umi_editor_workspace_edit_set_at(edits, i, &edit);
            if (status != UMI_STATUS_OK)
                break;
            printf("  Line %llu, column %llu: replace [%s] with [%s]\n",
                   (unsigned long long)edit.location.line, (unsigned long long)edit.location.column,
                   edit.expected_text, edit.replacement_text);
        }
    }
    if (status == UMI_STATUS_OK && argc == 2)
    {
        size_t applied;
        /* In an editor this branch belongs to the user's acceptance action.
         * The revision check remains required even after a successful preview. */
        status =
            umi_editor_workspace_edit_set_apply_document(edits, "file:///lesson/main.c", buffer, 1, &applied);
        if (status == UMI_STATUS_OK)
        {
            UmiEditorTextBufferView view;
            status = umi_editor_text_buffer_view(buffer, &view);
            if (status == UMI_STATUS_OK)
            {
                printf("Applied %zu edits to the lesson buffer:\n", applied);
                (void)fwrite(view.bytes, 1U, view.byte_count, stdout);
            }
        }
    }
    else if (status == UMI_STATUS_OK)
        puts("Preview only. Use --apply to change the in-memory lesson buffer.");
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "Completion review: %s\n", umi_status_text(status));
    umi_editor_workspace_edit_set_destroy(edits);
    UmiLanguageCompletionCatalogueDestroy(catalogue);
    umi_editor_text_buffer_destroy(buffer);
    return status == UMI_STATUS_OK ? 0 : 1;
}
