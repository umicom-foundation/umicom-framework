/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/workspace.h
 * PURPOSE: Commit creative edits and undo through the authoritative Data Server.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_WORKSPACE_WORKSPACE_H
#define UMICOM_CREATIVE_WORKSPACE_WORKSPACE_H
#include "umicom/creative_workspace/project.h"
#include "umicom/data/data_server.h"
#ifdef __cplusplus
extern "C" {
#endif
    typedef struct UmiCreativeWorkspace UmiCreativeWorkspace;
    /* One owner thread per workspace and exclusive transaction ownership of its
 * borrowed server during calls. The server must outlive the workspace. These
 * functions never join, commit or roll back another caller's transaction. */
    UmiStatus UmiCreativeWorkspaceCreate(UmiDataServer * server, const UmiCreativeProject * project,
    UmiCreativeWorkspace * * outWorkspace);
    UmiStatus UmiCreativeWorkspaceOpen(UmiDataServer * server, const char * projectId,
    UmiCreativeWorkspace * * outWorkspace);
    void UmiCreativeWorkspaceDestroy(UmiCreativeWorkspace * workspace);
    UmiStatus UmiCreativeWorkspaceCopy(const UmiCreativeWorkspace * workspace, UmiCreativeProject * outProject);
    /* Reload is explicit and discards only local undo/redo history, not saved
 * project data. It never creates a missing or corrupt project. */
    UmiStatus UmiCreativeWorkspaceReload(UmiCreativeWorkspace * workspace);
    /* An edit is all-or-nothing. Both expectedRevision and the candidate's source
 * revision must match. BUSY means reload and review, never overwrite silently.
 * Every accepted non-no-op edit is saved before it becomes visible. */
    UmiStatus UmiCreativeWorkspaceReplace(UmiCreativeWorkspace * workspace,
    const UmiCreativeProject * candidate, uint64_t expectedRevision);
    UmiStatus UmiCreativeWorkspaceUndo(UmiCreativeWorkspace * workspace, uint64_t expectedRevision);
    UmiStatus UmiCreativeWorkspaceRedo(UmiCreativeWorkspace * workspace, uint64_t expectedRevision);
    UmiStatus UmiCreativeWorkspaceHistory(const UmiCreativeWorkspace * workspace, UmiCreativeHistory * outHistory);
#ifdef __cplusplus
}

#endif
#endif
