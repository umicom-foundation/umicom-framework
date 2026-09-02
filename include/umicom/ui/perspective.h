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


/**
 * Represent the ui perspective snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiPerspectiveSnapshot {
    char perspective_id[UMI_UI_ID_CAPACITY];
    char title[UMI_UI_TEXT_CAPACITY];
    char description[UMI_UI_DESCRIPTION_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    int32_t order;
    int active;
} UmiUiPerspectiveSnapshot;

/**
 * Represent the ui perspective model data shared with callers of this public contract.
 */
typedef struct UmiUiPerspectiveModel UmiUiPerspectiveModel;

/**
 * Initialise ui perspective model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ui_perspective_model_create(UmiUiPerspectiveModel **out_model);
/**
 * Release or reset state held by ui perspective model so the same storage can be reused
 * safely.
 */
void umi_ui_perspective_model_destroy(UmiUiPerspectiveModel *model);
/**
 * Provide the ui perspective model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_perspective_model_upsert(UmiUiPerspectiveModel *model,
                                     const UmiUiPerspectiveSnapshot *item);
/**
 * Remove ui perspective model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_perspective_model_remove(UmiUiPerspectiveModel *model, const char *item_id);
/**
 * Find ui perspective model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_perspective_model_find(const UmiUiPerspectiveModel *model, const char *item_id,
                                   UmiUiPerspectiveSnapshot *out_item);
/**
 * Find ui perspective model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_perspective_model_at(const UmiUiPerspectiveModel *model, size_t index,
                                 UmiUiPerspectiveSnapshot *out_item);
/**
 * Return the number of records represented by ui perspective model without changing their
 * state.
 */
size_t umi_ui_perspective_model_count(const UmiUiPerspectiveModel *model);
/**
 * Provide the ui perspective model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_perspective_model_revision(const UmiUiPerspectiveModel *model);

#ifdef __cplusplus
}
#endif

#endif
