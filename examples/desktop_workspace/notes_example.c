/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/desktop_workspace/notes_example.c
 * PURPOSE:
 *   A complete memory-only example: edit a note, save two checkpoints, then restore the
 *   earlier note by creating a new checkpoint. No window is opened.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A complete memory-only example: edit a note, save two checkpoints, then
 * restore the earlier note by creating a new checkpoint. No window is opened.
 *---------------------------------------------------------------------------*/
#include "umicom/desktop_workspace/workspace.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    UmiDesktopWorkspace *workspace = NULL;
    UmiDesktopWorkspaceSnapshot *draft = calloc(1, sizeof *draft);
    if (!draft) return 1;
    UmiStatus status = UmiDesktopWorkspaceOpenMemory(&workspace);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceRead(workspace, draft);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspacePutNote(draft, "workshop", "Workshop plan", "Prepare the Notes exercise.");
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceCommit(workspace, draft->revision, draft);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceRead(workspace, draft);
    uint64_t first = draft->revision;
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspacePutNote(draft, "workshop", "Workshop plan", "Prepare the Notes and debugging exercises.");
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceCommit(workspace, draft->revision, draft);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceRead(workspace, draft);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceRestore(workspace, draft->revision, first);
    if (status == UMI_STATUS_OK) status = UmiDesktopWorkspaceRead(workspace, draft);
    if (status == UMI_STATUS_OK) {
        printf("Checkpoint %" PRIu64 ": %s\n%s\n", draft->revision, draft->notes[0].title, draft->notes[0].body);
        status = UmiDesktopWorkspaceCloseClean(workspace);
    }
    UmiDesktopWorkspaceDestroy(workspace); free(draft);
    if (status != UMI_STATUS_OK) { fprintf(stderr, "Practice stopped with status %d.\n", (int)status); return 1; }
    puts("Practice complete. The workspace stayed in memory; no application, guest or network was started.");
    return 0;
}
