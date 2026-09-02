/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/inspector_property.h
 *
 * PURPOSE:
 *   Describe an editable property row for enterprise inspectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_INSPECTOR_PROPERTY_H
#define UMICOM_UI_ENTERPRISE_INSPECTOR_PROPERTY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui ent inspector property data shared with callers of this public
 * contract.
 */
typedef struct UmiUiEntInspectorProperty {
    char property_id[UMI_UI_ENT_ID_CAPACITY];
    char section_id[UMI_UI_ENT_ID_CAPACITY];
    char label[UMI_UI_ENT_TEXT_CAPACITY];
    char value[UMI_UI_ENT_VALUE_CAPACITY];
    char value_type[UMI_UI_ENT_ID_CAPACITY];
    int editable;
    int required;
} UmiUiEntInspectorProperty;
/**
 * Initialise ui ent inspector property from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_ent_inspector_property_init(UmiUiEntInspectorProperty *value);
/**
 * Check that ui ent inspector property satisfies its contract before another service
 * relies on it.
 */
int umi_ui_ent_inspector_property_validate(const UmiUiEntInspectorProperty *value);
#ifdef __cplusplus
}
#endif

#endif
