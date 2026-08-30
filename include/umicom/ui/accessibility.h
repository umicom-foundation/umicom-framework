/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/accessibility.h
 *
 * PURPOSE:
 *   Define accessible names, descriptions, roles, states and parent relationships
 *   that all frontend adapters must preserve.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ACCESSIBILITY_H
#define UMICOM_UI_ACCESSIBILITY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_ACCESSIBILITY_MAX 256U


typedef struct UmiUiAccessibilitySnapshot {
    char node_id[UMI_UI_ID_CAPACITY];
    char parent_id[UMI_UI_ID_CAPACITY];
    char role_name[UMI_UI_ID_CAPACITY];
    char accessible_name[UMI_UI_TEXT_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    int enabled;
    int focused;
    int selected;
    int expanded;
} UmiUiAccessibilitySnapshot;

typedef struct UmiUiAccessibilityModel UmiUiAccessibilityModel;

UmiStatus umi_ui_accessibility_model_create(UmiUiAccessibilityModel **out_model);
void umi_ui_accessibility_model_destroy(UmiUiAccessibilityModel *model);
UmiStatus umi_ui_accessibility_model_upsert(UmiUiAccessibilityModel *model,
                                     const UmiUiAccessibilitySnapshot *item);
UmiStatus umi_ui_accessibility_model_remove(UmiUiAccessibilityModel *model, const char *item_id);
UmiStatus umi_ui_accessibility_model_find(const UmiUiAccessibilityModel *model, const char *item_id,
                                   UmiUiAccessibilitySnapshot *out_item);
UmiStatus umi_ui_accessibility_model_at(const UmiUiAccessibilityModel *model, size_t index,
                                 UmiUiAccessibilitySnapshot *out_item);
size_t umi_ui_accessibility_model_count(const UmiUiAccessibilityModel *model);
uint64_t umi_ui_accessibility_model_revision(const UmiUiAccessibilityModel *model);

#ifdef __cplusplus
}
#endif

#endif
