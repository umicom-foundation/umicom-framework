/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/publication.h
 * PURPOSE: Render an explicitly reviewed read-only website without publishing private board fields.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_KANBAN_PUBLICATION_H
#define UMICOM_KANBAN_PUBLICATION_H
#include "umicom/kanban/board.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_KANBAN_PUBLIC_SUMMARY_SIZE 1024U
#define UMI_KANBAN_PUBLICATION_BYTE_LIMIT 8388608U
/* Every displayed string is supplied specifically for publication. The source
 * card ID selects its current stage but is never printed. Private descriptions,
 * assignee IDs, comments, dependencies, labels, activity and paths are omitted. */
typedef struct UmiKanbanPublicCard {
    char card_id[UMI_KANBAN_ID_SIZE];
    char title[256];
    char summary[UMI_KANBAN_PUBLIC_SUMMARY_SIZE];
    char developer[128]; /* Empty means no developer name is published. */
} UmiKanbanPublicCard;
/* Render a complete UTF-8 HTML document with embedded responsive CSS and no
 * scripts, external assets, network requests, forms or editing endpoints.
 * The caller reviews the public title and selected cards before saving/uploading.
 * Only active, unique existing card IDs may be selected. An empty selection is
 * valid and never exposes other board data. Text must be bounded valid UTF-8.
 *
 * All access must be serialized on the board's owner thread. expected_revision
 * is the revision reviewed by the caller. Refusal never writes output bytes.
 * A NULL bytes/zero capacity call measures required bytes INCLUDING the trailing
 * NUL. A short buffer sets out_size to that requirement and otherwise changes
 * nothing. On success the host writes out_size - 1 bytes to an HTML file using
 * its chosen storage policy. No filesystem, upload or authentication occurs.
 * Inputs, board and outputs must not overlap. */
UmiStatus umi_kanban_publication_html(const UmiKanbanBoard *board,
    uint64_t expected_revision, const char *public_title,
    const UmiKanbanPublicCard *cards, size_t count,
    char *bytes, size_t capacity, size_t *out_size);
#ifdef __cplusplus
}
#endif
#endif
