/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/theme.h
 *
 * PURPOSE:
 *   Define semantic theme tokens so product code does not depend on toolkit-specific
 *   CSS selectors, colours or widget classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_THEME_H
#define UMICOM_UI_THEME_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"
#include "umicom/ui/value.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_THEME_MAX 256U

/**
 * Represent the ui theme snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiThemeSnapshot {
    char token_id[UMI_UI_ID_CAPACITY];
    char value[UMI_UI_VALUE_STRING_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
} UmiUiThemeSnapshot;

/**
 * Represent the ui theme model data shared with callers of this public contract.
 */
typedef struct UmiUiThemeModel UmiUiThemeModel;

/**
 * Initialise ui theme model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_theme_model_create(UmiUiThemeModel **out_model);
/**
 * Release or reset state held by ui theme model so the same storage can be reused safely.
 */
void umi_ui_theme_model_destroy(UmiUiThemeModel *model);
/**
 * Provide the ui theme model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_theme_model_upsert(UmiUiThemeModel *model,
                                     const UmiUiThemeSnapshot *item);
/**
 * Remove ui theme model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_theme_model_remove(UmiUiThemeModel *model, const char *item_id);
/**
 * Find ui theme model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_theme_model_find(const UmiUiThemeModel *model, const char *item_id,
                                   UmiUiThemeSnapshot *out_item);
/**
 * Find ui theme model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_theme_model_at(const UmiUiThemeModel *model, size_t index,
                                 UmiUiThemeSnapshot *out_item);
/**
 * Return the number of records represented by ui theme model without changing their state.
 */
size_t umi_ui_theme_model_count(const UmiUiThemeModel *model);
/**
 * Provide the ui theme model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_theme_model_revision(const UmiUiThemeModel *model);

#ifdef __cplusplus
}
#endif

#endif
