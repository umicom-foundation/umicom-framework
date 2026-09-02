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


/**
 * Represent the ui accessibility snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the ui accessibility model data shared with callers of this public contract.
 */
typedef struct UmiUiAccessibilityModel UmiUiAccessibilityModel;

/**
 * Initialise ui accessibility model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_accessibility_model_create(UmiUiAccessibilityModel **out_model);
/**
 * Release or reset state held by ui accessibility model so the same storage can be reused
 * safely.
 */
void umi_ui_accessibility_model_destroy(UmiUiAccessibilityModel *model);
/**
 * Provide the ui accessibility model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_accessibility_model_upsert(UmiUiAccessibilityModel *model,
                                     const UmiUiAccessibilitySnapshot *item);
/**
 * Remove ui accessibility model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_accessibility_model_remove(UmiUiAccessibilityModel *model, const char *item_id);
/**
 * Find ui accessibility model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_accessibility_model_find(const UmiUiAccessibilityModel *model, const char *item_id,
                                   UmiUiAccessibilitySnapshot *out_item);
/**
 * Find ui accessibility model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_accessibility_model_at(const UmiUiAccessibilityModel *model, size_t index,
                                 UmiUiAccessibilitySnapshot *out_item);
/**
 * Return the number of records represented by ui accessibility model without changing
 * their state.
 */
size_t umi_ui_accessibility_model_count(const UmiUiAccessibilityModel *model);
/**
 * Provide the ui accessibility model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_accessibility_model_revision(const UmiUiAccessibilityModel *model);

#ifdef __cplusplus
}
#endif

#endif
