/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/publication_plan_storage.c
 *
 * PURPOSE:
 *   Use rooted-file storage for complete private plan reads and atomic replacements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/kanban/publication_plan_storage.h"
#include "umicom/platform/rooted_files.h"
#include <stdlib.h>
#include <string.h>
static UmiStatus Location(const char *directory, const char *filename) {
  if (filename == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t n = strlen(filename);
  if (n <= 10U || n >= 256U || strcmp(filename + n - 10U, ".umipublic") || strchr(filename, '/') ||
      strchr(filename, '\\'))
    return UMI_STATUS_INVALID_ARGUMENT;
  return UmiRootedFileValidatePath(directory, filename);
}
UmiStatus UmiKanbanPublicationPlanLoad(const char *directory, const char *filename,
                                       UmiKanbanPublicationPlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  UmiStatus status = Location(directory, filename);
  unsigned char *bytes = NULL;
  size_t size = 0U;
  if (status == UMI_STATUS_OK)
    status =
        UmiRootedFileRead(directory, filename, UMI_KANBAN_PUBLIC_PLAN_MAXIMUM_BYTES, &bytes, &size);
  if (status == UMI_STATUS_OK)
    status = UmiKanbanPublicationPlanDecode(bytes, size, out);
  UmiRootedFileFree(bytes);
  return status;
}
UmiStatus UmiKanbanPublicationPlanSave(const char *directory, const char *filename,
                                       const UmiKanbanPublicationPlan *plan) {
  UmiStatus status = Location(directory, filename);
  size_t size = 0U;
  if (status == UMI_STATUS_OK)
    status = UmiKanbanPublicationPlanEncode(plan, NULL, 0U, &size);
  unsigned char *bytes = NULL;
  if (status == UMI_STATUS_OK) {
    bytes = malloc(size);
    if (bytes == NULL)
      status = UMI_STATUS_OUT_OF_MEMORY;
  }
  if (status == UMI_STATUS_OK)
    status = UmiKanbanPublicationPlanEncode(plan, bytes, size, &size);
  if (status == UMI_STATUS_OK)
    status = UmiRootedFileWrite(directory, filename, bytes, size);
  free(bytes);
  return status;
}
