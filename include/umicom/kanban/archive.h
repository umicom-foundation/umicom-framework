/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/archive.h
 * PURPOSE: Exchange bounded planning data without importing execution authority or local history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_KANBAN_ARCHIVE_H
#define UMICOM_KANBAN_ARCHIVE_H
#include "umicom/kanban/board.h"
#ifdef __cplusplus
extern "C" {
#endif
/* The existing value-archive envelope checks accidental corruption; it is not
 * encryption or authentication. Board text may be private. The caller chooses
 * storage and sharing; these functions perform no file or network operations.
 * Measure with NULL/zero. A short buffer reports required size but writes no
 * bytes. Other refusals preserve out_size and buffer. Serialize board access.
 */
UmiStatus umi_kanban_board_encode(const UmiKanbanBoard *board, uint64_t expected_revision,
    void *bytes, size_t capacity, size_t *out_size);
/* Decode into a new independent owner at revision 1 with empty local activity.
 * out_saved_revision is historical information, not a live revision token.
 * Reject unknown schemas, invalid text/counts, cycles, bad WIP/completion state,
 * trailing bytes and damaged/truncated input. Refusal preserves both outputs.
 * Successful callers own out_board and must destroy it. All storage is bounded
 * by MAX_CARDS and the shared value-archive byte limit; no large stack scratch.
 */
UmiStatus umi_kanban_board_decode(const void *bytes, size_t size,
    UmiKanbanBoard **out_board, uint64_t *out_saved_revision);
#ifdef __cplusplus
}
#endif
#endif
