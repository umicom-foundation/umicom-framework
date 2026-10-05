/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/quick_open_selection.c
 * PURPOSE: Revalidate complete file choices through the existing coherent index page service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer_workbench/quick_open_selection.h"
#include <stdlib.h>
#include <string.h>
static int QuickOpenTerminated(const char *text, size_t capacity)
{
    return memchr(text, '\0', capacity) != NULL;
}
UmiStatus UmiQuickOpenCheckSelection(const UmiFileIndex *index, const char *query,
                                     const UmiFileIndexPage *page, size_t position,
                                     const UmiFileIndexEntry *entry)
{
    if (index == NULL || query == NULL || page == NULL || entry == NULL || page->stats.revision == 0U ||
        position >= page->count || page->offset > page->matched ||
        page->count > page->matched - page->offset ||
        !QuickOpenTerminated(page->stats.root, sizeof(page->stats.root)) ||
        !QuickOpenTerminated(entry->path, sizeof(entry->path)) ||
        !QuickOpenTerminated(entry->relative_path, sizeof(entry->relative_path)) ||
        !QuickOpenTerminated(entry->name, sizeof(entry->name)) ||
        !QuickOpenTerminated(entry->extension, sizeof(entry->extension)))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Allocate the full path record away from the native UI stack. ReadPage
     * checks the revision while holding the same lock that copies the row. */
    UmiFileIndexEntry *current = calloc(1U, sizeof(*current));
    if (current == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiFileIndexPage actual;
    UmiStatus status = UmiFileIndexReadPage(index, query, 0, page->offset + position, page->stats.revision,
                                            current, 1U, &actual);
    if (status == UMI_STATUS_OK &&
        (actual.count != 1U || actual.matched != page->matched ||
         strcmp(actual.stats.root, page->stats.root) != 0 || strcmp(current->path, entry->path) != 0 ||
         strcmp(current->relative_path, entry->relative_path) != 0 ||
         strcmp(current->name, entry->name) != 0 || strcmp(current->extension, entry->extension) != 0 ||
         current->size != entry->size || current->modified_nanoseconds != entry->modified_nanoseconds))
        status = UMI_STATUS_BUSY;
    free(current);
    return status;
}
