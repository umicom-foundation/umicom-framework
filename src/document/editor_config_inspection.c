/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/editor_config_inspection.c
 *
 * PURPOSE:
 *   Inspect project-bounded editor settings through checked file reads with source digests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/editor_config_inspection.h"
#include "umicom/document/editor_config_evidence.h"
#include "umicom/platform/path.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiEditorConfigInspection {
  UmiEditorConfigInspectionScope scope;
  UmiEditorConfigResult *result;
  UmiEditorConfigInspectionFile files[UMI_EDITOR_CONFIG_MAXIMUM_LAYERS];
  size_t count;
};
void UmiEditorConfigInspectionDestroy(UmiEditorConfigInspection *i) {
  if (i) {
    UmiEditorConfigResultDestroy(i->result);
    free(i);
  }
}
const UmiEditorConfigResult *UmiEditorConfigInspectionResult(const UmiEditorConfigInspection *i) {
  return i ? i->result : NULL;
}
size_t UmiEditorConfigInspectionFileCount(const UmiEditorConfigInspection *i) {
  return i ? i->count : 0U;
}
UmiStatus UmiEditorConfigInspectionFileAt(const UmiEditorConfigInspection *i, size_t index,
                                          UmiEditorConfigInspectionFile *out) {
  if (!i || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= i->count)
    return UMI_STATUS_NOT_FOUND;
  *out = i->files[index];
  return UMI_STATUS_OK;
}
static int Cancelled(const UmiCancellationToken *c) {
  return c && umi_cancellation_token_is_requested(c);
}
UmiStatus UmiEditorConfigInspectionLoad(const char *root, const char *relative,
                                        const UmiCancellationToken *cancel,
                                        UmiEditorConfigInspection **out,
                                        UmiEditorConfigInspectionDiagnostic *diagnostic) {
  if (out)
    *out = NULL;
  UmiEditorConfigInspectionDiagnostic d = {{0}, 0U, UMI_STATUS_OK};
  UmiEditorConfig *configs[UMI_EDITOR_CONFIG_MAXIMUM_LAYERS] = {0};
  size_t prefixes[UMI_EDITOR_CONFIG_MAXIMUM_LAYERS] = {0};
  UmiEditorConfigInspection *inspection = NULL;
  UmiStatus status;
  if (!out || !root || !relative) {
    status = UMI_STATUS_INVALID_ARGUMENT;
    goto done;
  }
  status = UmiRootedFileValidatePath(root, relative);
  if (status != UMI_STATUS_OK)
    goto done;
  size_t length = strlen(relative);
  if (length >= UMI_EDITOR_CONFIG_INSPECTION_PATH) {
    status = UMI_STATUS_CAPACITY_EXCEEDED;
    goto done;
  }
  if (Cancelled(cancel)) {
    status = UMI_STATUS_CANCELLED;
    goto done;
  }
  inspection = calloc(1U, sizeof *inspection);
  if (!inspection) {
    status = UMI_STATUS_OUT_OF_MEMORY;
    goto done;
  }
  status = umi_path_normalise(root, inspection->scope.project_root,
                              sizeof inspection->scope.project_root);
  if (status != UMI_STATUS_OK)
    goto done;
  memcpy(inspection->scope.relative_file, relative, length + 1U);
  size_t prefix = length;
  while (prefix && relative[prefix - 1U] != '/')
    --prefix;
  size_t total = 0U, visited = 0U;
  for (;;) {
    if (++visited > UMI_EDITOR_CONFIG_MAXIMUM_LAYERS) {
      status = UMI_STATUS_CAPACITY_EXCEEDED;
      goto done;
    }
    if (Cancelled(cancel)) {
      status = UMI_STATUS_CANCELLED;
      goto done;
    }
    if (prefix + sizeof ".editorconfig" > sizeof d.relative_path) {
      status = UMI_STATUS_CAPACITY_EXCEEDED;
      goto done;
    }
    memcpy(d.relative_path, relative, prefix);
    memcpy(d.relative_path + prefix, ".editorconfig", sizeof ".editorconfig");
    unsigned char *bytes = NULL;
    size_t count = 0U;
    status =
        UmiRootedFileRead(root, d.relative_path, UMI_EDITOR_CONFIG_MAXIMUM_BYTES, &bytes, &count);
    if (status == UMI_STATUS_OK) {
      if (count > UMI_EDITOR_CONFIG_INSPECTION_BUDGET - total) {
        UmiRootedFileFree(bytes);
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto done;
      }
      total += count;
      size_t index = inspection->count;
      UmiEditorConfigDiagnostic parsed = {0};
      status = UmiEditorConfigParse((const char *)bytes, count, &configs[index], &parsed);
      d.line = parsed.line;
      if (status == UMI_STATUS_OK)
        status = UmiSha256Buffer(bytes, count, inspection->files[index].sha256);
      UmiRootedFileFree(bytes);
      if (status != UMI_STATUS_OK)
        goto done;
      prefixes[index] = prefix;
      UmiEditorConfigInspectionFile *file = &inspection->files[index];
      memcpy(file->relative_path, d.relative_path, strlen(d.relative_path) + 1U);
      file->bytes = count;
      file->root = UmiEditorConfigIsRoot(configs[index]);
      ++inspection->count;
      if (file->root)
        break;
    } else if (status != UMI_STATUS_NOT_FOUND)
      goto done;
    if (prefix == 0U)
      break;
    --prefix;
    while (prefix && relative[prefix - 1U] != '/')
      --prefix;
  }
  /* Resolve in inheritance order while every parsed owner is still alive.
   * Result strings are copied before those temporary trees are released. */
  UmiEditorConfigLayer layers[UMI_EDITOR_CONFIG_MAXIMUM_LAYERS];
  for (size_t i = 0; i < inspection->count; ++i) {
    size_t reverse = inspection->count - 1U - i;
    layers[i] = (UmiEditorConfigLayer){configs[reverse], relative + prefixes[reverse],
                                       inspection->files[reverse].relative_path};
  }
  /* Resolution may fail in any layer; do not attribute it to the last read. */
  d.relative_path[0] = 0;
  d.line = 0;
  status = UmiEditorConfigResolve(layers, inspection->count, &inspection->result);
  if (status != UMI_STATUS_OK)
    goto done;
  if (Cancelled(cancel)) {
    status = UMI_STATUS_CANCELLED;
    goto done;
  }
  for (size_t i = 0; i < inspection->count / 2U; ++i) {
    size_t other = inspection->count - 1U - i;
    UmiEditorConfigInspectionFile temp = inspection->files[i];
    inspection->files[i] = inspection->files[other];
    inspection->files[other] = temp;
  }
  *out = inspection;
  inspection = NULL;
  d.relative_path[0] = 0;
  d.line = 0;
done:
  for (size_t i = 0; i < UMI_EDITOR_CONFIG_MAXIMUM_LAYERS; ++i)
    UmiEditorConfigDestroy(configs[i]);
  UmiEditorConfigInspectionDestroy(inspection);
  d.status = status;
  if (diagnostic)
    *diagnostic = d;
  return status;
}

/* Scope is copied from the validated request and never inferred from display labels. */
UmiStatus UmiEditorConfigInspectionCopyScope(const UmiEditorConfigInspection *inspection,
                                             UmiEditorConfigInspectionScope *out) {
  if (inspection == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = inspection->scope;
  return UMI_STATUS_OK;
}
