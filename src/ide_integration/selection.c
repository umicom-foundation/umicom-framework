/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/selection.c
 *
 * PURPOSE:
 *   Implement bounded editor selection state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/selection.h"

#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    if (out == NULL || text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

void umi_ide_editor_selection_init(UmiIdeEditorSelection *selection)
{
    if (selection != NULL) {
        (void)memset(selection, 0, sizeof(*selection));
    }
}

UmiStatus umi_ide_editor_selection_set(
    UmiIdeEditorSelection *selection,
    const char *document_id,
    const char *path,
    const char *language_id,
    const char *text,
    uint32_t start_line,
    uint32_t start_column,
    uint32_t end_line,
    uint32_t end_column,
    uint64_t document_revision,
    int dirty)
{
    UmiStatus status;

    if (selection == NULL || document_id == NULL ||
        path == NULL || language_id == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(selection, 0, sizeof(*selection));

    status = copy_text(
        selection->document_id,
        sizeof(selection->document_id),
        document_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            selection->path,
            sizeof(selection->path),
            path);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            selection->language_id,
            sizeof(selection->language_id),
            language_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            selection->text,
            sizeof(selection->text),
            text);
    }
    if (status != UMI_STATUS_OK) return status;

    selection->start_line = start_line;
    selection->start_column = start_column;
    selection->end_line = end_line;
    selection->end_column = end_column;
    selection->document_revision = document_revision;
    selection->dirty = dirty != 0;

    return umi_ide_editor_selection_validate(selection);
}

UmiStatus umi_ide_editor_selection_validate(
    const UmiIdeEditorSelection *selection)
{
    if (selection == NULL ||
        selection->document_id[0] == '\0' ||
        selection->path[0] == '\0' ||
        selection->language_id[0] == '\0' ||
        selection->start_line == 0U ||
        selection->end_line < selection->start_line ||
        (selection->end_line == selection->start_line &&
         selection->end_column < selection->start_column)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return UMI_STATUS_OK;
}
