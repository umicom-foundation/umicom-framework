/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/code_action_catalogue.c
 * PURPOSE: Keep complete action proposals and decode only the selected supported text edit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/code_action_catalogue.h"
#include "umicom/language_runtime/response_tree.h"
#include "completion_catalogue_internal.h"
#include <stdlib.h>
#include <string.h>
typedef struct CodeActionRow
{
    char *title, *kind, *disabled_reason, *command;
    int item, edit, preferred, disabled, has_command, has_data;
} CodeActionRow;
struct UmiLanguageCodeActionCatalogue
{
    UmiJsonTree *tree;
    CodeActionRow *rows;
    size_t count;
};
/* Allocate in proportion to the actual JSON spelling, capped by the decoded
 * field limit. Short titles do not reserve an entire maximum-size buffer. */
static UmiStatus ActionText(const UmiJsonTree *tree, int node, size_t limit, int nonempty, char **out)
{
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    const char *span = NULL;
    size_t bytes = 0U;
    UmiStatus status = UmiJsonTreeSourceSpan(tree, node, &span, &bytes);
    (void)span;
    if (status != UMI_STATUS_OK)
        return status;
    size_t capacity = bytes < limit ? bytes + 1U : limit + 1U;
    char *text = malloc(capacity);
    if (text == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiJsonTreeText(tree, node, text, capacity);
    if (status == UMI_STATUS_OK && nonempty && text[0] == '\0')
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        *out = text;
    else
        free(text);
    return status;
}
static UmiStatus ActionArguments(const UmiJsonTree *tree, int object)
{
    int arguments = -1;
    UmiStatus status = CompletionMember(tree, object, "arguments", &arguments);
    if (status == UMI_STATUS_OK && arguments >= 0 &&
        UmiJsonTreeKind(tree, arguments) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    return status;
}
static UmiStatus ActionCommand(const UmiJsonTree *tree, int object, char **out)
{
    int title = -1, command = -1;
    char *caption = NULL;
    UmiStatus status = CompletionMember(tree, object, "title", &title);
    if (status == UMI_STATUS_OK)
        status = ActionText(tree, title, 65536U, 1, &caption);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, object, "command", &command);
    if (status == UMI_STATUS_OK)
        status = ActionText(tree, command, 4096U, 1, out);
    if (status == UMI_STATUS_OK)
        status = ActionArguments(tree, object);
    free(caption);
    return status;
}
static UmiStatus ActionRead(const UmiJsonTree *tree, int item, CodeActionRow *row)
{
    int title = -1, kind = -1, preferred = -1, disabled = -1, command = -1, data = -1, diagnostics = -1;
    row->item = item;
    row->edit = -1;
    UmiStatus status = CompletionMember(tree, item, "title", &title);
    if (status == UMI_STATUS_OK)
        status = ActionText(tree, title, 65536U, 1, &row->title);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, item, "command", &command);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, item, "kind", &kind);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, item, "isPreferred", &preferred);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, item, "disabled", &disabled);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, item, "edit", &row->edit);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, item, "data", &data);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, item, "diagnostics", &diagnostics);
    if (status != UMI_STATUS_OK)
        return status;
    row->has_data = data >= 0;
    row->has_command = command >= 0;
    row->disabled = disabled >= 0;
    if (UmiJsonTreeKind(tree, command) == UMI_LANGUAGE_RUNTIME_JSON_STRING)
    {
        /* A bare Command has a string command field. Reject a mixed literal
         * instead of discarding an edit or a disabled flag attached to it. */
        if (kind >= 0 || preferred >= 0 || disabled >= 0 || row->edit >= 0 || data >= 0 || diagnostics >= 0)
            return UMI_STATUS_PARSE_ERROR;
        return ActionCommand(tree, item, &row->command);
    }
    if (command >= 0)
        status = ActionCommand(tree, command, &row->command);
    if (status == UMI_STATUS_OK && kind >= 0)
        status = ActionText(tree, kind, 4096U, 0, &row->kind);
    if (status == UMI_STATUS_OK && preferred >= 0)
        status = UmiJsonTreeBoolean(tree, preferred, &row->preferred);
    if (status == UMI_STATUS_OK && disabled >= 0)
    {
        int reason = -1;
        status = CompletionMember(tree, disabled, "reason", &reason);
        if (status == UMI_STATUS_OK)
            status = ActionText(tree, reason, 65536U, 0, &row->disabled_reason);
    }
    if (status == UMI_STATUS_OK && row->edit >= 0 &&
        UmiJsonTreeKind(tree, row->edit) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK && diagnostics >= 0 &&
        UmiJsonTreeKind(tree, diagnostics) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
        status = UMI_STATUS_PARSE_ERROR;
    return status;
}
UmiStatus UmiLanguageCodeActionCatalogueCreate(const void *json, size_t bytes,
                                               const UmiCancellationToken *cancel,
                                               UmiLanguageCodeActionCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (json == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageCodeActionCatalogue *catalogue = calloc(1U, sizeof(*catalogue));
    if (catalogue == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    UmiStatus status = UmiJsonTreeCreate(json, bytes, &limits, cancel, &catalogue->tree);
    if (status == UMI_STATUS_OK && !UmiJsonTreeIsNull(catalogue->tree, 0))
    {
        if (UmiJsonTreeKind(catalogue->tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_ARRAY)
            status = UMI_STATUS_PARSE_ERROR;
        else
        {
            catalogue->count = UmiJsonTreeCount(catalogue->tree, 0);
            if (catalogue->count > 512U)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
            else
            {
                catalogue->rows =
                    calloc(catalogue->count == 0U ? 1U : catalogue->count, sizeof(*catalogue->rows));
                if (catalogue->rows == NULL)
                    status = UMI_STATUS_OUT_OF_MEMORY;
                int item = UmiJsonTreeFirst(catalogue->tree, 0);
                for (size_t i = 0U; status == UMI_STATUS_OK && i < catalogue->count;
                     ++i, item = UmiJsonTreeNext(catalogue->tree, item))
                {
                    if (CompletionCancelled(cancel))
                        status = UMI_STATUS_CANCELLED;
                    else
                        status = ActionRead(catalogue->tree, item, &catalogue->rows[i]);
                }
            }
        }
    }
    if (status == UMI_STATUS_OK && CompletionCancelled(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        *out = catalogue;
    else
        UmiLanguageCodeActionCatalogueDestroy(catalogue);
    return status;
}
UmiStatus UmiLanguageCodeActionCatalogueReadResponse(const void *json, size_t bytes, uint64_t id,
                                                     const UmiCancellationToken *cancel,
                                                     UmiLanguageCodeActionCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiJsonTree *tree = NULL;
    UmiJsonTreeLimits limits = {1024U * 1024U, 131072U, 32U};
    int result = -1;
    UmiStatus status = UmiLanguageResponseTreeRead(json, bytes, id, &limits, cancel, &tree, &result);
    const char *span = NULL;
    size_t length = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiJsonTreeSourceSpan(tree, result, &span, &length);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageCodeActionCatalogueCreate(span, length, cancel, out);
    UmiJsonTreeDestroy(tree);
    return status;
}
void UmiLanguageCodeActionCatalogueDestroy(UmiLanguageCodeActionCatalogue *catalogue)
{
    if (catalogue == NULL)
        return;
    if (catalogue->rows != NULL)
        for (size_t i = 0U; i < catalogue->count; ++i)
        {
            free(catalogue->rows[i].title);
            free(catalogue->rows[i].kind);
            free(catalogue->rows[i].disabled_reason);
            free(catalogue->rows[i].command);
        }
    free(catalogue->rows);
    UmiJsonTreeDestroy(catalogue->tree);
    free(catalogue);
}
size_t UmiLanguageCodeActionCatalogueCount(const UmiLanguageCodeActionCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
UmiStatus UmiLanguageCodeActionCatalogueAt(const UmiLanguageCodeActionCatalogue *catalogue, size_t index,
                                           UmiLanguageCodeAction *out)
{
    if (catalogue == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    const CodeActionRow *row = &catalogue->rows[index];
    *out = (UmiLanguageCodeAction){row->title,
                                   row->kind == NULL ? "" : row->kind,
                                   row->disabled_reason == NULL ? "" : row->disabled_reason,
                                   row->command == NULL ? "" : row->command,
                                   row->preferred,
                                   row->disabled,
                                   row->edit >= 0,
                                   row->has_command,
                                   row->has_data};
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageCodeActionCatalogueItemJson(const UmiLanguageCodeActionCatalogue *catalogue,
                                                 size_t index, const char **out_json, size_t *out_bytes)
{
    if (out_json != NULL)
        *out_json = NULL;
    if (out_bytes != NULL)
        *out_bytes = 0U;
    if (catalogue == NULL || out_json == NULL || out_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    return UmiJsonTreeSourceSpan(catalogue->tree, catalogue->rows[index].item, out_json, out_bytes);
}
UmiStatus UmiLanguageCodeActionCatalogueReadEdits(const UmiLanguageCodeActionCatalogue *catalogue,
                                                  size_t index, const UmiCancellationToken *cancel,
                                                  UmiLanguageWorkspaceEditCatalogue **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (catalogue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= catalogue->count)
        return UMI_STATUS_NOT_FOUND;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    const CodeActionRow *row = &catalogue->rows[index];
    if (row->disabled)
        return UMI_STATUS_PERMISSION_DENIED;
    if (row->has_command || row->edit < 0)
        return UMI_STATUS_NOT_IMPLEMENTED;
    const char *span = NULL;
    size_t bytes = 0U;
    UmiStatus status = UmiJsonTreeSourceSpan(catalogue->tree, row->edit, &span, &bytes);
    if (status == UMI_STATUS_OK)
        status = UmiLanguageWorkspaceEditCatalogueCreate(span, bytes, cancel, out);
    return status;
}

#include "code_action_filter.inc"

#include "code_action_resolution.inc"
