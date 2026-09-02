/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/pane.h
 *
 * PURPOSE:
 *   Define workbench pane metadata, placement, visibility and sizing without exposing
 *   GTK, Qt, Wt or browser widget types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_PANE_H
#define UMICOM_UI_PANE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_PANE_MAX 256U


/**
 * Represent the ui pane snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiPaneSnapshot {
    char pane_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char view_type[UMI_UI_ID_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    UmiUiPlacement placement;
    int32_t order;
    int visible;
    int closable;
    int movable;
    UmiUiSize preferred_size;
} UmiUiPaneSnapshot;

/**
 * Represent the ui pane model data shared with callers of this public contract.
 */
typedef struct UmiUiPaneModel UmiUiPaneModel;

/**
 * Initialise ui pane model from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_pane_model_create(UmiUiPaneModel **out_model);
/**
 * Release or reset state held by ui pane model so the same storage can be reused safely.
 */
void umi_ui_pane_model_destroy(UmiUiPaneModel *model);
/**
 * Provide the ui pane model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_pane_model_upsert(UmiUiPaneModel *model,
                                     const UmiUiPaneSnapshot *item);
/**
 * Remove ui pane model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_pane_model_remove(UmiUiPaneModel *model, const char *item_id);
/**
 * Find ui pane model while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_ui_pane_model_find(const UmiUiPaneModel *model, const char *item_id,
                                   UmiUiPaneSnapshot *out_item);
/**
 * Find ui pane model while leaving the underlying catalogue or model owned by this module.
 */
UmiStatus umi_ui_pane_model_at(const UmiUiPaneModel *model, size_t index,
                                 UmiUiPaneSnapshot *out_item);
/**
 * Return the number of records represented by ui pane model without changing their state.
 */
size_t umi_ui_pane_model_count(const UmiUiPaneModel *model);
/**
 * Provide the ui pane model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_pane_model_revision(const UmiUiPaneModel *model);

#ifdef __cplusplus
}
#endif

#endif
