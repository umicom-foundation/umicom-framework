/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/workspace_rename.h
 *
 * PURPOSE:
 *   Rename workspace entries without replacing existing destinations or changing editor state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PLATFORM_WORKSPACE_RENAME_H
#define UMICOM_PLATFORM_WORKSPACE_RENAME_H
#include "umicom/platform/workspace_graph.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWorkspaceRenameResult {
  char source[UMI_PATH_CAPACITY];
  char destination[UMI_PATH_CAPACITY];
  int renamed;
  int directory;
} UmiWorkspaceRenameResult;

/** Resolve a portable source path and a single new basename in the same folder.
 * No filesystem changes occur. The graph must be open, trusted and at the
 * supplied nonzero revision. Zero accepts its current revision. Registered
 * project roots and their ancestors cannot be renamed by this API.
 * The result is cleared before validation; success only fills the paths.
 * This preview grants no lasting permission: RenameEntry repeats validation. */
UmiStatus UmiWorkspaceRenamePreview(const UmiWorkspaceGraph *graph, uint64_t expectedRevision,
                                    const char *relativeSource, const char *newName,
                                    UmiWorkspaceRenameResult *outResult);

/** Rename one regular file or directory in its existing parent. Destination
 * replacement is forbidden by the OS operation, including a concurrently
 * created destination. Source links/reparse points and traversed linked
 * parents are rejected. Windows and Linux are implemented; unsupported
 * kernels/platforms return NOT_IMPLEMENTED, with no copy/delete fallback.
 *
 * The caller must first exclude all open documents at both paths, including
 * descendants of renamed directories, and serialize its project owners.
 * This does not rewrite build files, imports, Git paths, references or indexes.
 * It does not grant filesystem isolation against other processes: control
 * workspace directories and avoid concurrent external source/parent renames.
 * On Linux the source name is checked then renamed relative to its opened
 * parent, not pinned to an inode across concurrent external replacement.
 * The graph is borrowed on the caller's thread and remains alive throughout.
 * renamed is authoritative even if closing an OS handle subsequently fails.
 * Never repeat a successful rename merely because later index refresh fails. */
UmiStatus UmiWorkspaceRenameEntry(const UmiWorkspaceGraph *graph, uint64_t expectedRevision,
                                  const char *relativeSource, const char *newName,
                                  UmiWorkspaceRenameResult *outResult);
#ifdef __cplusplus
}
#endif
#endif
