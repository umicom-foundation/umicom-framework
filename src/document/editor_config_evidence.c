/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/editor_config_evidence.c
 *
 * PURPOSE:
 *   Compare immutable editor configuration observations without treating them as filesystem locks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/editor_config_evidence.h"
#include "umicom/platform/path.h"
#include <string.h>
UmiStatus UmiEditorConfigInspectionEqual(const UmiEditorConfigInspection *left,
                                         const UmiEditorConfigInspection *right, int *out) {
  if (left == NULL || right == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiEditorConfigInspectionScope a, b;
  UmiStatus status = UmiEditorConfigInspectionCopyScope(left, &a);
  if (status == UMI_STATUS_OK)
    status = UmiEditorConfigInspectionCopyScope(right, &b);
  if (status != UMI_STATUS_OK)
    return status;
  int equal =
      umi_path_equal(a.project_root, b.project_root) && !strcmp(a.relative_file, b.relative_file);
  size_t count = UmiEditorConfigInspectionFileCount(left);
  if (count != UmiEditorConfigInspectionFileCount(right))
    equal = 0;
  for (size_t i = 0U; equal && i < count; ++i) {
    UmiEditorConfigInspectionFile x, y;
    status = UmiEditorConfigInspectionFileAt(left, i, &x);
    if (status == UMI_STATUS_OK)
      status = UmiEditorConfigInspectionFileAt(right, i, &y);
    if (status != UMI_STATUS_OK)
      return status;
    equal = x.bytes == y.bytes && x.root == y.root && !strcmp(x.relative_path, y.relative_path) &&
            !strcmp(x.sha256, y.sha256);
  }
  const UmiEditorConfigResult *x = UmiEditorConfigInspectionResult(left),
                              *y = UmiEditorConfigInspectionResult(right);
  count = UmiEditorConfigResultCount(x);
  if (count != UmiEditorConfigResultCount(y))
    equal = 0;
  /* Compare copied effective properties too. Source identity alone must not
   * make different matching targets appear interchangeable to callers. */
  for (size_t i = 0U; equal && i < count; ++i) {
    UmiEditorConfigProperty px, py;
    status = UmiEditorConfigResultAt(x, i, &px);
    if (status == UMI_STATUS_OK)
      status = UmiEditorConfigResultAt(y, i, &py);
    if (status != UMI_STATUS_OK)
      return status;
    equal = px.line == py.line && !strcmp(px.key, py.key) && !strcmp(px.value, py.value) &&
            !strcmp(px.source_name, py.source_name) && !strcmp(px.pattern, py.pattern);
  }
  *out = equal;
  return UMI_STATUS_OK;
}
