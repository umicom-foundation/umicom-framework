/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/view_container.h
 *
 * PURPOSE:
 *   Group related workbench views behind one Activity Bar destination.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * View containers keep navigation structure in Framework metadata instead of hard-coding it in a Studio GTK window.
 */

#ifndef UMICOM_UI_VIEW_CONTAINER_H
#define UMICOM_UI_VIEW_CONTAINER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_VIEW_CONTAINER_MAX 64U
#define UMI_UI_VIEW_CONTAINER_MAX_VIEWS 32U

typedef struct UmiUiViewContainerSnapshot {
    char container_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    UmiUiPlacement placement;
    int32_t order;
    uint32_t badge_count;
    int visible;
    int active;
    char view_ids[UMI_UI_VIEW_CONTAINER_MAX_VIEWS][UMI_UI_ID_CAPACITY];
    size_t view_count;
    char active_view_id[UMI_UI_ID_CAPACITY];
} UmiUiViewContainerSnapshot;

typedef struct UmiUiViewContainerModel UmiUiViewContainerModel;

UmiStatus umi_ui_view_container_model_create(UmiUiViewContainerModel **out_model);
void umi_ui_view_container_model_destroy(UmiUiViewContainerModel *model);
UmiStatus umi_ui_view_container_model_upsert(UmiUiViewContainerModel *model,
                                             const UmiUiViewContainerSnapshot *item);
UmiStatus umi_ui_view_container_model_remove(UmiUiViewContainerModel *model,
                                             const char *container_id);
UmiStatus umi_ui_view_container_model_find(const UmiUiViewContainerModel *model,
                                           const char *container_id,
                                           UmiUiViewContainerSnapshot *out_item);
UmiStatus umi_ui_view_container_model_at(const UmiUiViewContainerModel *model,
                                         size_t index,
                                         UmiUiViewContainerSnapshot *out_item);
UmiStatus umi_ui_view_container_model_set_active(UmiUiViewContainerModel *model,
                                                 const char *container_id);
UmiStatus umi_ui_view_container_model_set_active_view(UmiUiViewContainerModel *model,
                                                      const char *container_id,
                                                      const char *view_id);
size_t umi_ui_view_container_model_count(const UmiUiViewContainerModel *model);
uint64_t umi_ui_view_container_model_revision(const UmiUiViewContainerModel *model);

#ifdef __cplusplus
}
#endif
#endif
