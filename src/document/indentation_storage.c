/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/indentation_storage.c
 *
 * PURPOSE:
 *   Reuse bounded input, exclusive staging and application path resolution for editor preference
 * records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/indentation_storage.h"
#include "umicom/platform/input_file.h"
#include "umicom/platform/local_replace.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/resource_location.h"
#include <stdlib.h>
#include <string.h>
UmiStatus UmiDocumentIndentationStoragePath(const char *application, const char *base, char *out,
                                            size_t capacity) {
  if (out == NULL || capacity == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiApplicationPathsConfig config = UmiApplicationPathsConfigDefault(application);
  config.baseOverride = base;
  UmiApplicationPaths *paths = calloc(1U, sizeof *paths);
  if (paths == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = UmiApplicationPathsResolve(&config, paths);
  char path[UMI_PATH_CAPACITY];
  if (status == UMI_STATUS_OK)
    status = umi_path_join(paths->config, "editor-indentation.txt", path, sizeof path);
  if (status == UMI_STATUS_OK)
    status = UmiOutputFileValidatePath(path);
  if (status == UMI_STATUS_OK) {
    size_t length = strlen(path);
    if (length >= capacity)
      status = UMI_STATUS_CAPACITY_EXCEEDED;
    else
      memcpy(out, path, length + 1U);
  }
  free(paths);
  return status;
}
UmiStatus UmiDocumentIndentationStorageLoad(const char *path, UmiEditorIndentation *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  unsigned char *bytes = NULL;
  size_t size = 0U;
  UmiStatus status =
      UmiInputFileRead(path, UMI_EDITOR_INDENTATION_RECORD_MAXIMUM_BYTES, &bytes, &size);
  if (status == UMI_STATUS_OK)
    status = UmiEditorIndentationDecode(bytes, size, out);
  UmiInputFileFree(bytes);
  return status;
}
UmiStatus UmiDocumentIndentationStorageSave(const char *path, const UmiEditorIndentation *options) {
  char encoded[UMI_EDITOR_INDENTATION_RECORD_MAXIMUM_BYTES];
  size_t size = 0U;
  UmiStatus status = UmiEditorIndentationEncode(options, encoded, sizeof encoded, &size);
  return status == UMI_STATUS_OK ? UmiLocalFileReplace(path, encoded, size) : status;
}
