/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/editor_config_adoption.c
 *
 * PURPOSE:
 *   Bind project configuration evidence to a named working copy and checked indentation override.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/editor_config_adoption.h"
#include "umicom/platform/path.h"
#include <stdlib.h>
#include <string.h>
struct UmiDocumentEditorConfigPlan {
  UmiDocumentEditorConfigSummary summary;
  UmiDocumentIndentationReview review;
  UmiEditorConfigInspection *inspection;
};
static UmiStatus AdoptionDocument(const UmiDocumentCoordinator *owner, UmiDocumentId id,
                                  UmiDocumentWorkingCopySnapshot *out) {
  for (size_t i = 0U; i < umi_document_coordinator_count(owner); ++i) {
    UmiDocumentWorkingCopySnapshot item;
    UmiStatus status = umi_document_coordinator_at(owner, i, &item);
    if (status != UMI_STATUS_OK)
      return status;
    if (item.document_id == id) {
      *out = item;
      return UMI_STATUS_OK;
    }
  }
  return UMI_STATUS_NOT_FOUND;
}
void UmiDocumentEditorConfigPlanDestroy(UmiDocumentEditorConfigPlan *plan) {
  if (plan) {
    UmiEditorConfigInspectionDestroy(plan->inspection);
    free(plan);
  }
}
UmiStatus UmiDocumentEditorConfigPlanCreate(UmiDocumentCoordinator *owner, UmiDocumentId id,
                                            UmiEditorConfigInspection *inspection,
                                            UmiDocumentEditorConfigPlan **out) {
  if (out)
    *out = NULL;
  if (!owner || !id || !inspection || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDocumentWorkingCopySnapshot document;
  UmiStatus status = AdoptionDocument(owner, id, &document);
  if (status != UMI_STATUS_OK)
    return status;
  if (!document.has_path)
    return UMI_STATUS_INVALID_STATE;
  UmiEditorConfigInspectionScope scope;
  status = UmiEditorConfigInspectionCopyScope(inspection, &scope);
  char absolute[UMI_PATH_CAPACITY];
  if (status == UMI_STATUS_OK)
    status = umi_path_join(scope.project_root, scope.relative_file, absolute, sizeof absolute);
  if (status != UMI_STATUS_OK)
    return status;
  if (!umi_path_equal(absolute, document.path))
    return UMI_STATUS_INVALID_STATE;
  UmiDocumentIndentationReview review;
  status = UmiDocumentCoordinatorInspectIndentation(owner, id, &review);
  if (status != UMI_STATUS_OK)
    return status;
  const UmiEditorConfigResult *result = UmiEditorConfigInspectionResult(inspection);
  UmiEditorConfigSettings settings;
  status = UmiEditorConfigSettingsRead(result, &settings);
  if (status != UMI_STATUS_OK)
    return status;
  const char *keys[] = {"indent_style", "indent_size", "tab_width"};
  const unsigned flags[] = {UMI_EDITOR_CONFIG_INDENT_STYLE, UMI_EDITOR_CONFIG_INDENT_SIZE,
                            UMI_EDITOR_CONFIG_TAB_WIDTH};
  size_t applicable = 0U;
  for (size_t i = 0U; i < 3U; ++i) {
    UmiEditorConfigProperty property;
    status = UmiEditorConfigResultFind(result, keys[i], &property);
    if (status == UMI_STATUS_OK) {
      if (!(settings.present & flags[i]))
        return UMI_STATUS_PARSE_ERROR;
      ++applicable;
    } else if (status != UMI_STATUS_NOT_FOUND)
      return status;
  }
  if (applicable == 0U)
    return UMI_STATUS_NOT_FOUND;
  UmiEditorIndentation candidate;
  status = UmiEditorConfigSettingsIndentation(result, &review.effective, &candidate);
  if (status != UMI_STATUS_OK)
    return status;
  UmiDocumentEditorConfigPlan *plan = calloc(1U, sizeof *plan);
  if (!plan)
    return UMI_STATUS_OUT_OF_MEMORY;
  plan->review = review;
  plan->summary.document_id = id;
  plan->summary.scope = scope;
  memcpy(plan->summary.document_path, document.path, strlen(document.path) + 1U);
  memcpy(plan->summary.display_name, document.display_name, strlen(document.display_name) + 1U);
  plan->summary.previous = review.effective;
  plan->summary.proposed = candidate;
  plan->summary.indentation_properties =
      settings.present & (UMI_EDITOR_CONFIG_INDENT_STYLE | UMI_EDITOR_CONFIG_INDENT_SIZE |
                          UMI_EDITOR_CONFIG_TAB_WIDTH);
  plan->summary.inspected_files = UmiEditorConfigInspectionFileCount(inspection);
  plan->summary.ignored_properties = UmiEditorConfigResultCount(result) - applicable;
  plan->summary.invalid_values = settings.invalid_values;
  plan->summary.unknown_properties = settings.unknown_properties;
  plan->summary.already_overridden = review.overridden;
  /* Transfer only after complete validation. The plan retains immutable source
   * evidence so callers can release temporary parser and UI state independently. */
  plan->inspection = inspection;
  *out = plan;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentEditorConfigPlanInspect(const UmiDocumentEditorConfigPlan *plan,
                                             UmiDocumentEditorConfigSummary *out) {
  if (!plan || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = plan->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiDocumentEditorConfigPlanSource(const UmiDocumentEditorConfigPlan *plan, size_t index,
                                            UmiEditorConfigInspectionFile *out) {
  if (!plan)
    return UMI_STATUS_INVALID_ARGUMENT;
  return UmiEditorConfigInspectionFileAt(plan->inspection, index, out);
}
UmiStatus UmiDocumentEditorConfigPlanCheck(const UmiDocumentEditorConfigPlan *plan,
                                           const UmiDocumentCoordinator *owner) {
  if (!plan || !owner)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (plan->summary.applied)
    return UMI_STATUS_ALREADY_EXISTS;
  UmiStatus status = UmiDocumentCoordinatorCheckIndentation(owner, &plan->review);
  if (status != UMI_STATUS_OK)
    return status;
  UmiDocumentWorkingCopySnapshot document;
  status = AdoptionDocument(owner, plan->summary.document_id, &document);
  if (status != UMI_STATUS_OK)
    return status;
  return document.has_path && umi_path_equal(document.path, plan->summary.document_path)
             ? UMI_STATUS_OK
             : UMI_STATUS_INVALID_STATE;
}
UmiStatus UmiDocumentEditorConfigPlanApplyObserved(UmiDocumentEditorConfigPlan *plan,
                                                   UmiDocumentCoordinator *owner,
                                                   const UmiEditorConfigInspection *observed) {
  if (!plan || !owner || !observed)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiDocumentEditorConfigPlanCheck(plan, owner);
  if (status != UMI_STATUS_OK)
    return status;
  int equal = 0;
  status = UmiEditorConfigInspectionEqual(plan->inspection, observed, &equal);
  if (status != UMI_STATUS_OK)
    return status;
  if (!equal)
    return UMI_STATUS_INVALID_STATE;
  status =
      UmiDocumentCoordinatorApplyIndentation(owner, &plan->review, &plan->summary.proposed, NULL);
  if (status == UMI_STATUS_OK)
    plan->summary.applied = 1;
  return status;
}
