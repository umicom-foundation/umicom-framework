/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/kanban/document.h
 * PURPOSE: Own an editable planning board at an explicit local location and preserve edits when storage changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_KANBAN_DOCUMENT_H
#define UMICOM_KANBAN_DOCUMENT_H
#include <stdbool.h>
#include "umicom/kanban/board.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_KANBAN_FILENAME_CAPACITY 128U
typedef struct UmiKanbanDocument UmiKanbanDocument;
typedef struct UmiKanbanDocumentState {
    char directory[UMI_PATH_CAPACITY];
    char filename[UMI_KANBAN_FILENAME_CAPACITY];
    uint64_t board_revision;
    uint64_t saved_live_revision;
    uint64_t archive_revision;
    bool unsaved_changes;
} UmiKanbanDocumentState;
typedef enum UmiKanbanSaveProblem {
    UMI_KANBAN_SAVE_NONE,
    UMI_KANBAN_SAVE_ARGUMENT,
    UMI_KANBAN_SAVE_STALE_REVISION,
    UMI_KANBAN_SAVE_EXTERNAL_CHANGE,
    UMI_KANBAN_SAVE_STORAGE
} UmiKanbanSaveProblem;
/* The host supplies an existing absolute trusted directory and a single ordinary
 * filename ending in .umikb (lowercase). No default home/AppData folder, parent
 * creation or network service is used. Windows paths are UTF-8 drive paths;
 * POSIX paths use native bytes. Links, traversal and device names are refused.
 * The directory and its ancestors must remain stable for the session lifetime.
 *
 * Create refuses an existing file. Open validates the complete archive before
 * exposing a board. out_document must address NULL; refusal preserves it.
 * Each session owns its board, baseline bytes and a native lease on the sibling
 * filename + ".lock". Keep this persistent guard file, including after crashes
 * or failed opens. All cooperating writers must follow this convention. It is
 * not a distributed lock or protection against unrelated tools. Operations may
 * block: use an application worker and serialize all access to the session.
 * New files use exclusive creation, including against unrelated writers. A
 * failed initial write may leave an incomplete new file; preserve it for review
 * and choose another name rather than silently deleting or replacing it.
 * Contents are plaintext planning data; the user controls storage permissions.
 */
UmiStatus umi_kanban_document_create(const char *directory, const char *filename,
    const UmiKanbanConfig *config, UmiKanbanDocument **out_document);
UmiStatus umi_kanban_document_open(const char *directory, const char *filename,
    UmiKanbanDocument **out_document);
/* Borrow the board for the existing checked edit API. Never destroy it or use
 * it after document close. No other thread may edit during save/read/close. */
UmiKanbanBoard *umi_kanban_document_board(UmiKanbanDocument *document);
/* In-memory information only. unsaved_changes does not assert that the external
 * file is unchanged. Reopened live revisions start at 1; archive_revision is
 * historical information, not an edit token or proof of verified execution. */
UmiStatus umi_kanban_document_state(const UmiKanbanDocument *document,
    UmiKanbanDocumentState *out_state);
/* Check expected_revision before I/O, compare complete disk bytes with the
 * baseline, then publish a complete sibling replacement. External differences
 * (including a missing/oversized file) refuse the save and retain in-memory
 * edits. A clean save still checks disk but does not rewrite the archive.
 * No automatic reload, merge or force overwrite occurs. The native lease
 * excludes cooperating writers; an unrelated writer can still race between
 * comparison and publication. This is not a filesystem transaction against
 * arbitrary tools. Directory power-loss durability is not guaranteed.
 * out_problem is optional. Other document state is unchanged on refusal. */
UmiStatus umi_kanban_document_save(UmiKanbanDocument *document, uint64_t expected_revision,
    UmiKanbanSaveProblem *out_problem);
/* Recover/export current edits to a different new file. Refuse an existing
 * destination, retain the original binding and dirty state, and never delete
 * either file. The new file uses the same persistent-guard convention. A close
 * error can be reported after the complete copy has been written: inspect the
 * destination before retrying; do not replace it blindly. */
UmiStatus umi_kanban_document_save_copy(const UmiKanbanDocument *document,
    uint64_t expected_revision, const char *directory, const char *filename);
/* No implicit save. Refuse dirty closure unless discard_changes is true. A
 * successful close consumes the pointer. A native lease close error also
 * consumes it, returning IO_ERROR. The archive and guard remain on disk. */
UmiStatus umi_kanban_document_close(UmiKanbanDocument **document, bool discard_changes);
#ifdef __cplusplus
}
#endif
#endif
