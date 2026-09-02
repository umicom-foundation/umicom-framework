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

/**
 * Represent the ui view container snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the ui view container model data shared with callers of this public contract.
 */
typedef struct UmiUiViewContainerModel UmiUiViewContainerModel;

/**
 * Initialise ui view container model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_view_container_model_create(UmiUiViewContainerModel **out_model);
/**
 * Release or reset state held by ui view container model so the same storage can be reused
 * safely.
 */
void umi_ui_view_container_model_destroy(UmiUiViewContainerModel *model);
/**
 * Provide the ui view container model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_container_model_upsert(UmiUiViewContainerModel *model,
                                             const UmiUiViewContainerSnapshot *item);
/**
 * Remove ui view container model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_view_container_model_remove(UmiUiViewContainerModel *model,
                                             const char *container_id);
/**
 * Find ui view container model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_view_container_model_find(const UmiUiViewContainerModel *model,
                                           const char *container_id,
                                           UmiUiViewContainerSnapshot *out_item);
/**
 * Find ui view container model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ui_view_container_model_at(const UmiUiViewContainerModel *model,
                                         size_t index,
                                         UmiUiViewContainerSnapshot *out_item);
/**
 * Provide the ui view container model set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_view_container_model_set_active(UmiUiViewContainerModel *model,
                                                 const char *container_id);
/**
 * Provide the ui view container model set active view operation used by this module and
 * its client applications.
 */
UmiStatus umi_ui_view_container_model_set_active_view(UmiUiViewContainerModel *model,
                                                      const char *container_id,
                                                      const char *view_id);
/**
 * Return the number of records represented by ui view container model without changing
 * their state.
 */
size_t umi_ui_view_container_model_count(const UmiUiViewContainerModel *model);
/**
 * Provide the ui view container model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_view_container_model_revision(const UmiUiViewContainerModel *model);

#ifdef __cplusplus
}
#endif
#endif
