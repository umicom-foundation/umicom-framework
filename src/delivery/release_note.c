/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/release_note.c
 *
 * PURPOSE:
 *   Store structured release-note entries without making documentation generation part of product code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Release notes can be rendered later from structured change entries rather than assembled manually from commit messages.
 */

#include "umicom/delivery/release_note.h"
#include "delivery_internal.h"
#include <string.h>
void umi_release_notes_init(UmiReleaseNotes *notes)
{
    if (notes != NULL) (void)memset(notes, 0, sizeof(*notes));
}
UmiStatus umi_release_notes_add(UmiReleaseNotes *notes,
                                const char *category,
                                const char *text)
{
    UmiReleaseNoteEntry *entry;
    if (notes == NULL || category == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (notes->count >= UMI_DELIVERY_MAX_METADATA) return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &notes->entries[notes->count++];
    (void)memset(entry, 0, sizeof(*entry));
    if (umi_delivery_copy_text(entry->category, sizeof(entry->category), category) != UMI_STATUS_OK ||
        umi_delivery_copy_text(entry->text, sizeof(entry->text), text) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
