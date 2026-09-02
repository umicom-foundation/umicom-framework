/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/inspector_editor.h
 *
 * PURPOSE:
 *   Describe the semantic editor used for an inspector property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_INSPECTOR_EDITOR_H
#define UMICOM_UI_ENTERPRISE_INSPECTOR_EDITOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent inspector editor data shared with callers of this public contract.
 */
typedef struct UmiUiEntInspectorEditor {
    char property_id[UMI_UI_ENT_ID_CAPACITY];
    char editor_kind[UMI_UI_ENT_ID_CAPACITY];
    size_t choice_count;
    int multiline;
    int read_only;
} UmiUiEntInspectorEditor;
/**
 * Initialise ui ent inspector editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_editor_init(UmiUiEntInspectorEditor *value);
/**
 * Check that ui ent inspector editor satisfies its contract before another service relies
 * on it.
 */
int umi_ui_ent_inspector_editor_validate(const UmiUiEntInspectorEditor *value);
#ifdef __cplusplus
}
#endif

#endif
