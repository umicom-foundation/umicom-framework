/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/editor_config_adoption.h
 *
 * PURPOSE:
 *   Review file-specific indentation proposals and apply only after matching configuration
 * evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_EDITOR_CONFIG_ADOPTION_H
#define UMICOM_DOCUMENT_EDITOR_CONFIG_ADOPTION_H
#include "umicom/document/editor_config_evidence.h"
#include "umicom/document/indentation_override.h"
#include "umicom/editor/editor_config_settings.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDocumentEditorConfigPlan UmiDocumentEditorConfigPlan;
typedef struct UmiDocumentEditorConfigSummary {
  UmiDocumentId document_id;
  UmiEditorConfigInspectionScope scope;
  char document_path[UMI_PATH_CAPACITY];
  char display_name[UMI_DOCUMENT_NAME_CAPACITY];
  UmiEditorIndentation previous, proposed;
  unsigned indentation_properties;
  size_t inspected_files, ignored_properties, invalid_values, unknown_properties;
  int already_overridden, applied;
} UmiDocumentEditorConfigSummary;
/* Capture a named, saved working copy and its effective indentation. The
 * inspection boundary+target must name that same path. Missing properties keep
 * current document values. At least one valid indentation property is needed.
 * Invalid indentation values return PARSE_ERROR; unrepresentable widths return
 * NOT_IMPLEMENTED. Other properties are counted but never applied here.
 * On success the plan takes ownership of inspection. On failure the caller
 * still owns it. No filesystem I/O, preference mutation or source changes.
 * Call on the coordinator owner thread and keep that coordinator alive. */
UmiStatus UmiDocumentEditorConfigPlanCreate(UmiDocumentCoordinator *coordinator,
                                            UmiDocumentId document_id,
                                            UmiEditorConfigInspection *inspection,
                                            UmiDocumentEditorConfigPlan **out_plan);
void UmiDocumentEditorConfigPlanDestroy(UmiDocumentEditorConfigPlan *plan);
UmiStatus UmiDocumentEditorConfigPlanInspect(const UmiDocumentEditorConfigPlan *plan,
                                             UmiDocumentEditorConfigSummary *out_summary);
UmiStatus UmiDocumentEditorConfigPlanSource(const UmiDocumentEditorConfigPlan *plan, size_t index,
                                            UmiEditorConfigInspectionFile *out_source);
/* Check that the captured owner, saved path and policy are still current.
 * Text/caret changes and activating another tab do not invalidate the plan. */
UmiStatus UmiDocumentEditorConfigPlanCheck(const UmiDocumentEditorConfigPlan *plan,
                                           const UmiDocumentCoordinator *coordinator);
/* Apply once, comparing a caller-supplied reinspection with the captured
 * evidence before checking the live document and applying its override.
 * Interactive callers re-read on a worker immediately before this owner-thread
 * operation. This function does not read/lock files or guarantee they have not
 * changed since the supplied observation. A changed/additional/removed config
 * returns INVALID_STATE without mutation. ALREADY_EXISTS means already applied.
 * No text, history, format, trimming, newline or disk state is changed. */
UmiStatus UmiDocumentEditorConfigPlanApplyObserved(UmiDocumentEditorConfigPlan *plan,
                                                   UmiDocumentCoordinator *coordinator,
                                                   const UmiEditorConfigInspection *reinspection);
#ifdef __cplusplus
}
#endif
#endif
