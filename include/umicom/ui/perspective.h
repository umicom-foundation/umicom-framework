/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/perspective.h
 *
 * PURPOSE:
 *   Define named workbench perspectives and their active state, title and ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_PERSPECTIVE_H
#define UMICOM_UI_PERSPECTIVE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_PERSPECTIVE_MAX 256U


typedef struct UmiUiPerspectiveSnapshot {
    char perspective_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    int32_t order;
    int active;
} UmiUiPerspectiveSnapshot;

typedef struct UmiUiPerspectiveModel UmiUiPerspectiveModel;

UmiStatus umi_ui_perspective_model_create(UmiUiPerspectiveModel **out_model);
void umi_ui_perspective_model_destroy(UmiUiPerspectiveModel *model);
UmiStatus umi_ui_perspective_model_upsert(UmiUiPerspectiveModel *model,
                                     const UmiUiPerspectiveSnapshot *item);
UmiStatus umi_ui_perspective_model_remove(UmiUiPerspectiveModel *model, const char *item_id);
UmiStatus umi_ui_perspective_model_find(const UmiUiPerspectiveModel *model, const char *item_id,
                                   UmiUiPerspectiveSnapshot *out_item);
UmiStatus umi_ui_perspective_model_at(const UmiUiPerspectiveModel *model, size_t index,
                                 UmiUiPerspectiveSnapshot *out_item);
size_t umi_ui_perspective_model_count(const UmiUiPerspectiveModel *model);
uint64_t umi_ui_perspective_model_revision(const UmiUiPerspectiveModel *model);

#ifdef __cplusplus
}
#endif

#endif
