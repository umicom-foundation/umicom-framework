/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/theme.h
 *
 * PURPOSE:
 *   Define semantic theme tokens so product code does not depend on toolkit-specific
 *   CSS selectors, colours or widget classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiUiThemeSnapshot {
    char token_id[UMI_UI_ID_CAPACITY];
    char value[UMI_UI_VALUE_STRING_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
} UmiUiThemeSnapshot;

typedef struct UmiUiThemeModel UmiUiThemeModel;

UmiStatus umi_ui_theme_model_create(UmiUiThemeModel **out_model);
void umi_ui_theme_model_destroy(UmiUiThemeModel *model);
UmiStatus umi_ui_theme_model_upsert(UmiUiThemeModel *model,
                                     const UmiUiThemeSnapshot *item);
UmiStatus umi_ui_theme_model_remove(UmiUiThemeModel *model, const char *item_id);
UmiStatus umi_ui_theme_model_find(const UmiUiThemeModel *model, const char *item_id,
                                   UmiUiThemeSnapshot *out_item);
UmiStatus umi_ui_theme_model_at(const UmiUiThemeModel *model, size_t index,
                                 UmiUiThemeSnapshot *out_item);
size_t umi_ui_theme_model_count(const UmiUiThemeModel *model);
uint64_t umi_ui_theme_model_revision(const UmiUiThemeModel *model);

#ifdef __cplusplus
}
#endif

#endif
