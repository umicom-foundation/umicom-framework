/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/workspace_rename.c
 *
 * PURPOSE:
 *   Authorize same-parent workspace renames and dispatch no-replacement platform operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _GNU_SOURCE
#endif
#include "umicom/platform/workspace_rename.h"
#include "workspace_name_internal.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
/* Native fragments use the platform types and functions below. Retain the
 * former ordering for review; the active include follows their declarations. */
#if 0
#include "workspace_rename_windows.inc"
#endif
#include <windows.h>
#include "workspace_rename_windows.inc"
#elif defined(__linux__)
/* As on Windows, system declarations must precede the native implementation.
 * Keep the former ordering disabled for review without changing rename logic. */
#if 0
#include "workspace_rename_linux.inc"
#endif
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "workspace_rename_linux.inc"
#else
static UmiStatus RenameNative(const char *root, const char *relative,
                              UmiWorkspaceRenameResult *result) {
  (void)root;
  (void)relative;
  (void)result;
  return UMI_STATUS_NOT_IMPLEMENTED;
}
#endif

/* The same lexical proposal is used by preview and execution. It never
 * normalises away traversal, metadata names or an absolute replacement. */
UmiStatus UmiWorkspaceRenamePreview(const UmiWorkspaceGraph *graph, uint64_t expectedRevision,
                                    const char *relativeSource, const char *newName,
                                    UmiWorkspaceRenameResult *outResult) {
  if (outResult == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  memset(outResult, 0, sizeof(*outResult));
  if (graph == NULL || newName == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiWorkspaceGraphSnapshot workspace;
  UmiStatus status = umi_workspace_graph_snapshot(graph, &workspace);
  if (status != UMI_STATUS_OK)
    return status;
  if (!workspace.open)
    return UMI_STATUS_INVALID_STATE;
  if (expectedRevision != 0U && expectedRevision != workspace.revision)
    return UMI_STATUS_BUSY;
  if (!workspace.trusted)
    return UMI_STATUS_PERMISSION_DENIED;
  char source[UMI_PATH_CAPACITY], name[UMI_PATH_CAPACITY], parent[UMI_PATH_CAPACITY];
  status = ValidateRelative(relativeSource, source);
  if (status != UMI_STATUS_OK)
    return status;
  status = ValidateRelative(newName, name);
  if (status != UMI_STATUS_OK)
    return status;
  if (strchr(name, '/') != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiWorkspaceRenameResult result = {0};
  status = umi_path_join(workspace.root, source, result.source, sizeof result.source);
  if (status == UMI_STATUS_OK)
    status = umi_path_parent(result.source, parent, sizeof parent);
  if (status == UMI_STATUS_OK)
    status = umi_path_join(parent, name, result.destination, sizeof result.destination);
  if (status != UMI_STATUS_OK)
    return status;
  if (umi_path_equal(result.source, result.destination))
    return UMI_STATUS_ALREADY_EXISTS;
  /* A registered project's identity belongs to the workspace graph. Renaming
   * it needs a separate project relocation workflow, not a file operation. */
  for (size_t i = 0U; i < workspace.project_count; ++i) {
    UmiWorkspaceProjectSnapshot project;
    status = umi_workspace_graph_project_at(graph, i, &project);
    if (status != UMI_STATUS_OK)
      return status;
    if (umi_path_equal(result.source, project.root) ||
        umi_path_is_within(result.source, project.root) ||
        umi_path_equal(result.destination, project.root) ||
        umi_path_is_within(result.destination, project.root))
      return UMI_STATUS_PERMISSION_DENIED;
  }
  *outResult = result;
  return UMI_STATUS_OK;
}

UmiStatus UmiWorkspaceRenameEntry(const UmiWorkspaceGraph *graph, uint64_t expectedRevision,
                                  const char *relativeSource, const char *newName,
                                  UmiWorkspaceRenameResult *outResult) {
  UmiStatus status =
      UmiWorkspaceRenamePreview(graph, expectedRevision, relativeSource, newName, outResult);
  if (status != UMI_STATUS_OK)
    return status;
  UmiWorkspaceGraphSnapshot workspace;
  status = umi_workspace_graph_snapshot(graph, &workspace);
  if (status != UMI_STATUS_OK)
    return status;
  char relative[UMI_PATH_CAPACITY];
  status = ValidateRelative(relativeSource, relative);
  if (status != UMI_STATUS_OK)
    return status;
  return RenameNative(workspace.root, relative, outResult);
}
