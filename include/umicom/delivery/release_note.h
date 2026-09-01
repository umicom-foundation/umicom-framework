/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release_note.h
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

#ifndef INCLUDE_UMICOM_DELIVERY_RELEASE_NOTE_H
#define INCLUDE_UMICOM_DELIVERY_RELEASE_NOTE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseNoteEntry {
    char category[UMI_DELIVERY_ID_CAPACITY];
    char text[UMI_DELIVERY_TEXT_CAPACITY];
} UmiReleaseNoteEntry;
typedef struct UmiReleaseNotes {
    UmiReleaseNoteEntry entries[UMI_DELIVERY_MAX_METADATA];
    size_t count;
} UmiReleaseNotes;
void umi_release_notes_init(UmiReleaseNotes *notes);
UmiStatus umi_release_notes_add(UmiReleaseNotes *notes,
                                const char *category,
                                const char *text);

#ifdef __cplusplus
}
#endif

#endif
