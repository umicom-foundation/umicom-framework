/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/history_entries.inc
 * PURPOSE: Transfer source and optional selection metadata together without allocating after a document commit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void history_clear(DocumentHistoryEntry *items, size_t *count)
{
    for (size_t i = 0U; i < *count; ++i)
    {
        free(items[i].text);
        items[i] = (DocumentHistoryEntry){0};
    }
    *count = 0U;
}
static int HistoryBoundary(const char *text, size_t bytes, size_t offset)
{
    return offset <= bytes && (offset == bytes || ((unsigned char)text[offset] & 0xc0U) != 0x80U) &&
           !(offset > 0U && offset < bytes && text[offset - 1U] == '\r' && text[offset] == '\n');
}
static void HistoryPushView(DocumentHistoryEntry *items, size_t *count, char *text,
                            const UmiUiDocumentViewSnapshot *view)
{
    size_t length = strlen(text), incoming = length + 1U, bytes = 0U;
    DocumentHistoryEntry entry = {0};
    entry.text = text;
    /* Coordinates are meaningful only for this exact text. Callers pass NULL
     * when a pending typed draft differs from the stored state being retained. */
    if (view != NULL && view->cursor_offset <= length &&
        view->selection_length <= length - view->cursor_offset &&
        HistoryBoundary(text, length, view->cursor_offset) &&
        HistoryBoundary(text, length, view->cursor_offset + view->selection_length))
    {
        entry.cursor_offset = view->cursor_offset;
        entry.selection_length = view->selection_length;
        entry.has_position = 1;
    }
    for (size_t i = 0U; i < *count; ++i)
        bytes += strlen(items[i].text) + 1U;
    while (*count != 0U && (*count == UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY ||
                            incoming > UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET - bytes))
    {
        bytes -= strlen(items[0].text) + 1U;
        free(items[0].text);
        memmove(items, items + 1U, (*count - 1U) * sizeof(*items));
        --*count;
    }
    items[(*count)++] = entry;
}
/* Text-only sources retain their established fallback positioning. */
static void HistoryPushOwned(DocumentHistoryEntry *items, size_t *count, char *text)
{
    HistoryPushView(items, count, text, NULL);
}
static UmiStatus history_push(DocumentHistoryEntry *items, size_t *count, const char *text)
{
    if (items == NULL || count == NULL || text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = strlen(text);
    if (bytes >= UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *copy = malloc(bytes + 1U);
    if (copy == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, text, bytes + 1U);
    HistoryPushOwned(items, count, copy);
    return UMI_STATUS_OK;
}
static char *history_pop(DocumentHistoryEntry *items, size_t *count)
{
    if (items == NULL || count == NULL || *count == 0U)
        return NULL;
    --*count;
    char *text = items[*count].text;
    items[*count] = (DocumentHistoryEntry){0};
    return text;
}
static void HistoryRestoreSelection(const DocumentHistoryEntry *entry, UmiUiDocumentViewSnapshot *view)
{
    size_t bytes = strlen(entry->text);
    if (entry->has_position)
    {
        view->cursor_offset = entry->cursor_offset;
        view->selection_length = entry->selection_length;
    }
    if (view->cursor_offset > bytes)
        view->cursor_offset = bytes;
    if (view->selection_length > bytes - view->cursor_offset)
        view->selection_length = bytes - view->cursor_offset;
    size_t end = view->cursor_offset + view->selection_length;
    /* Legacy text-only history can land inside a scalar or a CRLF pair.
     * Move each endpoint toward the preceding boundary without expanding it. */
    while (view->cursor_offset > 0U && !HistoryBoundary(entry->text, bytes, view->cursor_offset))
        --view->cursor_offset;
    while (end > view->cursor_offset && !HistoryBoundary(entry->text, bytes, end))
        --end;
    view->selection_length = end - view->cursor_offset;
}
