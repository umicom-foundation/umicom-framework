/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/selection.c
 *
 * PURPOSE:
 *   Implement bounded editor selection state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/selection.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || text == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ide editor selection from caller-provided values so later operations receive
 * a known state.
 */
void umi_ide_editor_selection_init(UmiIdeEditorSelection *selection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection != NULL) {
        (void)memset(selection, 0, sizeof(*selection));
    }
}

/*
 * Copy ide editor selection into module-owned storage so callers keep ownership of their
 * input values.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (selection == NULL || document_id == NULL ||
        path == NULL || language_id == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(selection, 0, sizeof(*selection));

    status = copy_text(
        selection->document_id,
        sizeof(selection->document_id),
        document_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            selection->path,
            sizeof(selection->path),
            path);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            selection->language_id,
            sizeof(selection->language_id),
            language_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            selection->text,
            sizeof(selection->text),
            text);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    selection->start_line = start_line;
    selection->start_column = start_column;
    selection->end_line = end_line;
    selection->end_column = end_column;
    selection->document_revision = document_revision;
    selection->dirty = dirty != 0;

    return umi_ide_editor_selection_validate(selection);
}

/*
 * Check that ide editor selection satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ide_editor_selection_validate(
    const UmiIdeEditorSelection *selection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
