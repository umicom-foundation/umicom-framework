/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/activity.h
 *
 * PURPOSE:
 *   Define the toolkit-neutral Activity Bar model used to switch major workbench areas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * An activity is a stable navigation choice such as Explorer, Search, Source Control or Designer. GTK4, Qt, Wt and headless adapters render the same records.
 */

#ifndef UMICOM_UI_ACTIVITY_H
#define UMICOM_UI_ACTIVITY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_ACTIVITY_MAX 64U

/**
 * Represent the ui activity snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiActivitySnapshot {
    char activity_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char icon_name[UMI_UI_ICON_CAPACITY];
    char container_id[UMI_UI_ID_CAPACITY];
    int32_t order;
    uint32_t badge_count;
    int visible;
    int enabled;
    int active;
} UmiUiActivitySnapshot;

/**
 * Represent the ui activity model data shared with callers of this public contract.
 */
typedef struct UmiUiActivityModel UmiUiActivityModel;

/**
 * Initialise ui activity model from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_activity_model_create(UmiUiActivityModel **out_model);
/**
 * Release or reset state held by ui activity model so the same storage can be reused
 * safely.
 */
void umi_ui_activity_model_destroy(UmiUiActivityModel *model);
/**
 * Provide the ui activity model upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_activity_model_upsert(UmiUiActivityModel *model,
                                       const UmiUiActivitySnapshot *item);
/**
 * Remove ui activity model while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ui_activity_model_remove(UmiUiActivityModel *model,
                                       const char *activity_id);
/**
 * Find ui activity model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_activity_model_find(const UmiUiActivityModel *model,
                                     const char *activity_id,
                                     UmiUiActivitySnapshot *out_item);
/**
 * Find ui activity model while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_activity_model_at(const UmiUiActivityModel *model,
                                   size_t index,
                                   UmiUiActivitySnapshot *out_item);
/**
 * Provide the ui activity model set active operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_activity_model_set_active(UmiUiActivityModel *model,
                                          const char *activity_id);
/**
 * Return the number of records represented by ui activity model without changing their
 * state.
 */
size_t umi_ui_activity_model_count(const UmiUiActivityModel *model);
/**
 * Provide the ui activity model revision operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_activity_model_revision(const UmiUiActivityModel *model);

#ifdef __cplusplus
}
#endif
#endif
