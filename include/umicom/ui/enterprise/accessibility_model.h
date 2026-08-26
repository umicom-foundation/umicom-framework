/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/accessibility_model.h
 *
 * PURPOSE:
 *   Describe accessible row/column metadata for virtualised enterprise views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ACCESSIBILITY_MODEL_H
#define UMICOM_UI_ENTERPRISE_ACCESSIBILITY_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiEntAccessibilityModel {
    char role[UMI_UI_ENT_ID_CAPACITY];
    char name[UMI_UI_ENT_TEXT_CAPACITY];
    char description[UMI_UI_ENT_TEXT_CAPACITY];
    size_t row_index;
    size_t column_index;
    size_t set_size;
    size_t position_in_set;
} UmiUiEntAccessibilityModel;
UmiStatus umi_ui_ent_accessibility_model_init(UmiUiEntAccessibilityModel *value);
int umi_ui_ent_accessibility_model_validate(const UmiUiEntAccessibilityModel *value);
#ifdef __cplusplus
}
#endif

#endif
