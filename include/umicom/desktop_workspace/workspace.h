/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_workspace/workspace.h
 * PURPOSE: Own persistent desktop notes, preferences and transactional checkpoints.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop_workspace/workspace.h
 * Purpose: Persistent desktop notes, presentation preferences and checkpoints.
 * Architecture: Framework owns state, validation and Data Server transactions.
 * A desktop shell only presents drafts and requests explicit commits.
 * Author: Sammy Hegab, Umicom Foundation | Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_WORKSPACE_H
#define UMICOM_DESKTOP_WORKSPACE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/data/data_server.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_DESKTOP_WORKSPACE_NOTES 16U
#define UMI_DESKTOP_WORKSPACE_ID 40U
#define UMI_DESKTOP_WORKSPACE_TITLE 96U
#define UMI_DESKTOP_WORKSPACE_BODY 4096U
#define UMI_DESKTOP_WORKSPACE_HISTORY 8U

typedef enum UmiDesktopWorkspaceTheme {
    UMI_DESKTOP_WORKSPACE_SYSTEM = 0,
    UMI_DESKTOP_WORKSPACE_LIGHT = 1,
    UMI_DESKTOP_WORKSPACE_DARK = 2
} UmiDesktopWorkspaceTheme;

typedef struct UmiDesktopWorkspaceNote {
    char id[UMI_DESKTOP_WORKSPACE_ID];
    char title[UMI_DESKTOP_WORKSPACE_TITLE];
    char body[UMI_DESKTOP_WORKSPACE_BODY];
} UmiDesktopWorkspaceNote;

typedef struct UmiDesktopWorkspaceSnapshot {
    uint64_t revision;
    UmiDesktopWorkspaceTheme theme;
    unsigned fontPoints;
    int sidebarVisible;
    char selectedNote[UMI_DESKTOP_WORKSPACE_ID];
    size_t noteCount;
    UmiDesktopWorkspaceNote notes[UMI_DESKTOP_WORKSPACE_NOTES];
} UmiDesktopWorkspaceSnapshot;

typedef struct UmiDesktopWorkspace UmiDesktopWorkspace;

/** Initialise a caller-owned empty draft. No filesystem or process activity. */
void UmiDesktopWorkspaceSnapshotInit(UmiDesktopWorkspaceSnapshot *snapshot);
UmiStatus UmiDesktopWorkspaceValidate(const UmiDesktopWorkspaceSnapshot *snapshot);
/** Edits change only the caller's draft. Inputs are copied; fields borrowed from the same draft
 * may be used as inputs. Identifiers are stable ASCII tokens; labels/body are
 * bounded UTF-8. Put selects the note. Remove selects a remaining note, if any. */
UmiStatus UmiDesktopWorkspacePutNote(UmiDesktopWorkspaceSnapshot *snapshot,
    const char *id, const char *title, const char *body);
UmiStatus UmiDesktopWorkspaceRemoveNote(UmiDesktopWorkspaceSnapshot *snapshot,
    const char *id);

/** Memory-only demonstration, using the canonical memory Data Server. */
UmiStatus UmiDesktopWorkspaceOpenMemory(UmiDesktopWorkspace **outWorkspace);
/** Open a dedicated existing or new workspace directory. Its parent must exist.
 * Files have fixed names: workspace.sqlite and workspace.lock. The native
 * adapter refuses links and obtains a nonblocking cooperating-process lock.
 * Linux requires a private directory owned by the current uid. Windows relies
 * on the caller's private directory ACL; it is not an ACL-management service.
 * SQLite unavailable means UNAVAILABLE: there is no silent memory fallback.
 * Opening records an active local session; it never launches applications,
 * changes the system desktop or writes outside the selected directory. */
UmiStatus UmiDesktopWorkspaceOpenDirectory(const char *directory,
    UmiDesktopWorkspace **outWorkspace);
/** Embedded use: caller retains a dedicated Data Server connection and must
 * serialise its lifetime against other users of this namespace. No concurrent
 * access to that connection is allowed until Destroy. Receipt compare-and-set
 * remains a second check, not a replacement for an external process lock. */
UmiStatus UmiDesktopWorkspaceOpenServer(UmiDataServer *server,
    UmiDesktopWorkspace **outWorkspace);
/** All objects are single-threaded. Callbacks must not re-enter them. A UI may
 * move an idle object to its sole worker, but never access it simultaneously. */
UmiStatus UmiDesktopWorkspaceRead(const UmiDesktopWorkspace *workspace,
    UmiDesktopWorkspaceSnapshot *outSnapshot);
uint64_t UmiDesktopWorkspaceOldestRevision(const UmiDesktopWorkspace *workspace);
int UmiDesktopWorkspacePreviousSessionUnfinished(const UmiDesktopWorkspace *workspace);
/** Commit is explicit. A stale expected revision is INVALID_STATE. The draft's
 * revision must agree. Only after Data Server commit succeeds is the in-memory
 * snapshot replaced. Up to eight complete checkpoints are retained; the oldest
 * is retired inside the SAME transaction. No user filesystem file is deleted.
 * Digests detect accidental record corruption; they are not authentication. */
UmiStatus UmiDesktopWorkspaceCommit(UmiDesktopWorkspace *workspace,
    uint64_t expectedRevision, const UmiDesktopWorkspaceSnapshot *draft);
UmiStatus UmiDesktopWorkspaceReadCheckpoint(UmiDesktopWorkspace *workspace,
    uint64_t revision, UmiDesktopWorkspaceSnapshot *outSnapshot);
/** Restore creates a NEW revision from a retained checkpoint. It does not undo
 * application transactions, terminate processes or overwrite an external file. */
UmiStatus UmiDesktopWorkspaceRestore(UmiDesktopWorkspace *workspace,
    uint64_t expectedRevision, uint64_t checkpointRevision);
/** Mark an explicitly completed session. On failure, leave the object alive
 * for diagnosis. No further mutations are accepted after a successful close.
 * Destroy does NOT mark clean: abnormal shutdown must remain observable. */
UmiStatus UmiDesktopWorkspaceCloseClean(UmiDesktopWorkspace *workspace);
void UmiDesktopWorkspaceDestroy(UmiDesktopWorkspace *workspace);
const char *UmiDesktopWorkspaceDetail(const UmiDesktopWorkspace *workspace);
int UmiDesktopWorkspaceMain(int argc, char **argv);
#ifdef __cplusplus
}
#endif
#endif
