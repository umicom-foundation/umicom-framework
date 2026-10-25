/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/editor_config_evidence.h
 *
 * PURPOSE:
 *   Identify and compare captured configuration scopes and their source evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DOCUMENT_EDITOR_CONFIG_EVIDENCE_H
#define UMICOM_DOCUMENT_EDITOR_CONFIG_EVIDENCE_H
#include "umicom/document/editor_config_inspection.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiEditorConfigInspectionScope {
  char project_root[UMI_EDITOR_CONFIG_INSPECTION_PATH];
  char relative_file[UMI_EDITOR_CONFIG_INSPECTION_PATH];
} UmiEditorConfigInspectionScope;
/* Copy the normalized absolute boundary and exact case-sensitive relative
 * matching input captured by Load. No filesystem access; output unchanged on error. */
UmiStatus UmiEditorConfigInspectionCopyScope(const UmiEditorConfigInspection *inspection,
                                             UmiEditorConfigInspectionScope *out_scope);
/* Compare the boundary, target matching path, ordered source digests and resolved
 * properties/provenance. Re-reading also detects a newly added closer config.
 * This compares observations; it neither reads current files nor locks them.
 * NULL observations are invalid. Output unchanged on error. */
UmiStatus UmiEditorConfigInspectionEqual(const UmiEditorConfigInspection *left,
                                         const UmiEditorConfigInspection *right, int *out_equal);
#ifdef __cplusplus
}
#endif
#endif
