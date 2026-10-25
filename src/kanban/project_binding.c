/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/project_binding.c
 *
 * PURPOSE:
 *   Validate private board bindings through the existing project and archive path rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/kanban/project_binding.h"
#include <string.h>
UmiStatus UmiKanbanProjectBindingCreate(const char *root, const char *directory,
                                        const char *filename, UmiKanbanProjectBinding *out) {
  if (out == NULL || root == NULL || directory == NULL || filename == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiKanbanProjectLocation project;
  UmiStatus status = UmiKanbanProjectLocationCreate(root, &project);
  if (status == UMI_STATUS_OK)
    status = UmiKanbanLocationValidate(directory, filename);
  UmiKanbanProjectBinding candidate = {0};
  if (status == UMI_STATUS_OK)
    status = umi_path_normalise(directory, candidate.directory, sizeof candidate.directory);
  if (status != UMI_STATUS_OK)
    return status;
  memcpy(candidate.project_root, project.project_root, strlen(project.project_root) + 1U);
  memcpy(candidate.filename, filename, strlen(filename) + 1U);
  *out = candidate;
  return UMI_STATUS_OK;
}
UmiStatus UmiKanbanProjectBindingValidate(const UmiKanbanProjectBinding *binding) {
  if (binding == NULL ||
      memchr(binding->project_root, '\0', sizeof binding->project_root) == NULL ||
      memchr(binding->directory, '\0', sizeof binding->directory) == NULL ||
      memchr(binding->filename, '\0', sizeof binding->filename) == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiKanbanProjectBinding candidate;
  UmiStatus status = UmiKanbanProjectBindingCreate(binding->project_root, binding->directory,
                                                   binding->filename, &candidate);
  if (status != UMI_STATUS_OK)
    return status;
  /* A canonical key and its stored project identity must agree exactly.
   * Case aliases and moved projects intentionally remain separate bindings. */
  return strcmp(candidate.project_root, binding->project_root) == 0 &&
                 strcmp(candidate.directory, binding->directory) == 0
             ? UMI_STATUS_OK
             : UMI_STATUS_INVALID_ARGUMENT;
}
