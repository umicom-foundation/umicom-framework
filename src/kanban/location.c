/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/kanban/location.c
 *
 * PURPOSE:
 *   Share board location admission and project naming without performing storage operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/kanban/location.h"
#include "umicom/base/sha256.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/rooted_files.h"
#include <string.h>
UmiStatus UmiKanbanLocationValidate(const char *directory, const char *filename) {
  if (filename == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = 0U;
  while (length < UMI_KANBAN_FILENAME_CAPACITY && filename[length] != '\0')
    ++length;
  if (length == UMI_KANBAN_FILENAME_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  if (length <= 6U || strcmp(filename + length - 6U, ".umikb") != 0 ||
      strchr(filename, '/') != NULL || strchr(filename, '\\') != NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiRootedFileValidatePath(directory, filename);
  if (status != UMI_STATUS_OK)
    return status;
  char guard_name[UMI_KANBAN_FILENAME_CAPACITY + 5U], guard[UMI_PATH_CAPACITY];
  memcpy(guard_name, filename, length);
  memcpy(guard_name + length, ".lock", 6U);
  status = umi_path_join(directory, guard_name, guard, sizeof guard);
  if (status == UMI_STATUS_OK)
    status = UmiOutputFileValidatePath(guard);
  return status;
}
UmiStatus UmiKanbanProjectLocationCreate(const char *root, UmiKanbanProjectLocation *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiKanbanLocationValidate(root, "development.umikb");
  if (status != UMI_STATUS_OK)
    return status;
  UmiKanbanProjectLocation value = {0};
  status = umi_path_normalise(root, value.project_root, sizeof value.project_root);
  if (status != UMI_STATUS_OK)
    return status;
  UmiSha256 hash;
  unsigned char digest[UMI_SHA256_BYTES];
  char hex[UMI_SHA256_HEX_CAPACITY];
  UmiSha256Init(&hash);
  const char domain[] = "umicom.kanban.project/1\n";
  status = UmiSha256Update(&hash, domain, sizeof domain - 1U);
  if (status == UMI_STATUS_OK)
    status = UmiSha256Update(&hash, value.project_root, strlen(value.project_root));
  if (status == UMI_STATUS_OK)
    status = UmiSha256Final(&hash, digest);
  if (status != UMI_STATUS_OK)
    return status;
  UmiSha256Hex(digest, hex);
  memcpy(value.filename, "project-", 8U);
  memcpy(value.filename + 8U, hex, 64U);
  memcpy(value.filename + 72U, ".umikb", 7U);
  *out = value;
  return UMI_STATUS_OK;
}
