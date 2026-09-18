/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/workspace_files.h
 *
 * PURPOSE:
 *   Create project files and folders through the authoritative workspace graph,
 *   preserving existing entries and rejecting stale or untrusted requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_WORKSPACE_FILES_H
#define UMICOM_PLATFORM_WORKSPACE_FILES_H

#include "umicom/platform/workspace_graph.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The New command creates an empty file or one directory, never a tree copy. */
typedef enum UmiWorkspaceEntryKind {
    UMI_WORKSPACE_ENTRY_FILE = 0,
    UMI_WORKSPACE_ENTRY_DIRECTORY = 1
} UmiWorkspaceEntryKind;

/** Copied outcome. created remains true if creation succeeded but closing the
 * new handle reported an error. Never delete or overwrite that entry on retry. */
typedef struct UmiWorkspaceEntryResult {
    char path[UMI_PATH_CAPACITY];
    int created;
} UmiWorkspaceEntryResult;

/** Create a new, empty project entry without replacing an existing destination.
 * The graph is borrowed and must remain on its owning thread throughout this
 * call. It must be open and trusted. expectedRevision=0 accepts current state;
 * a nonzero stale revision returns BUSY before touching the filesystem.
 *
 * relativePath uses portable file names. Absolute paths, parent traversal,
 * repository metadata (.git), device names and invalid Windows characters are
 * rejected on every platform. Parent folders must already exist. Symlinks and
 * Windows reparse points below the selected root are not followed.
 *
 * Creation uses an exclusive OS operation. A duplicate returns ALREADY_EXISTS,
 * not a successful overwrite. outResult is cleared before validation. It owns
 * its copied path and has no retained resources. No index, document, compiler,
 * authentication or execution state is changed by this call. Workspace trust
 * is a local-edit permission, not an operating-system security sandbox.
 */
UmiStatus UmiWorkspaceCreateEntry(const UmiWorkspaceGraph *graph,
    uint64_t expectedRevision, const char *relativePath,
    UmiWorkspaceEntryKind kind, UmiWorkspaceEntryResult *outResult);

#ifdef __cplusplus
}
#endif
#endif
