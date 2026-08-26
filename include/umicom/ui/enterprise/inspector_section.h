/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/inspector_section.h
 *
 * PURPOSE:
 *   Describe a collapsible property-inspector section.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_INSPECTOR_SECTION_H
#define UMICOM_UI_ENTERPRISE_INSPECTOR_SECTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiEntInspectorSection {
    char section_id[UMI_UI_ENT_ID_CAPACITY];
    char label[UMI_UI_ENT_TEXT_CAPACITY];
    int32_t order;
    int collapsed;
} UmiUiEntInspectorSection;
UmiStatus umi_ui_ent_inspector_section_init(UmiUiEntInspectorSection *value);
int umi_ui_ent_inspector_section_validate(const UmiUiEntInspectorSection *value);
#ifdef __cplusplus
}
#endif

#endif
