/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/binding_storage.c
 *
 * PURPOSE:
 *   Store checked project bindings without writing into project repositories or board archives.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/kanban/binding_storage.h"
#include "umicom/platform/rooted_files.h"
#include <string.h>
static UmiStatus BindingLeaf(const char *root, char leaf[96], UmiKanbanProjectLocation *project) {
  UmiStatus status = UmiKanbanProjectLocationCreate(root, project);
  if (status != UMI_STATUS_OK)
    return status;
  /* Reuse the existing root identity hash; the distinct suffix prevents a
   * settings record from being mistaken for a board archive. */
  memcpy(leaf, "board-", 6U);
  memcpy(leaf + 6U, project->filename + 8U, 64U);
  memcpy(leaf + 70U, ".umilocation", 13U);
  return UMI_STATUS_OK;
}
UmiStatus UmiKanbanBindingStoragePath(const char *settings, const char *root, char *out,
                                      size_t capacity) {
  if (out == NULL || capacity == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiKanbanProjectLocation project;
  char leaf[96], result[UMI_PATH_CAPACITY];
  UmiStatus status = BindingLeaf(root, leaf, &project);
  if (status == UMI_STATUS_OK)
    status = UmiRootedFileValidatePath(settings, leaf);
  if (status == UMI_STATUS_OK)
    status = umi_path_join(settings, leaf, result, sizeof result);
  if (status != UMI_STATUS_OK)
    return status;
  size_t length = strlen(result);
  if (length >= capacity)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  memcpy(out, result, length + 1U);
  return UMI_STATUS_OK;
}
UmiStatus UmiKanbanBindingStorageLoad(const char *settings, const char *root,
                                      UmiKanbanProjectBinding *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiKanbanProjectLocation project;
  char leaf[96];
  UmiStatus status = BindingLeaf(root, leaf, &project);
  unsigned char *bytes = NULL;
  size_t size = 0U;
  if (status == UMI_STATUS_OK)
    status = UmiRootedFileRead(settings, leaf, UMI_KANBAN_BINDING_MAXIMUM_BYTES, &bytes, &size);
  UmiKanbanProjectBinding candidate;
  if (status == UMI_STATUS_OK)
    status = UmiKanbanProjectBindingDecode(bytes, size, &candidate);
  if (status == UMI_STATUS_OK && strcmp(candidate.project_root, project.project_root) != 0)
    status = UMI_STATUS_INVALID_STATE;
  UmiRootedFileFree(bytes);
  if (status == UMI_STATUS_OK)
    *out = candidate;
  return status;
}
UmiStatus UmiKanbanBindingStorageSave(const char *settings,
                                      const UmiKanbanProjectBinding *binding) {
  UmiStatus status = UmiKanbanProjectBindingValidate(binding);
  if (status != UMI_STATUS_OK)
    return status;
  UmiKanbanProjectLocation project;
  char leaf[96];
  status = BindingLeaf(binding->project_root, leaf, &project);
  unsigned char bytes[UMI_KANBAN_BINDING_MAXIMUM_BYTES];
  size_t size = 0U;
  if (status == UMI_STATUS_OK)
    status = UmiKanbanProjectBindingEncode(binding, bytes, sizeof bytes, &size);
  if (status == UMI_STATUS_OK)
    status = UmiRootedFileWrite(settings, leaf, bytes, size);
  return status;
}
