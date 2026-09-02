/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/view_model.h
 *
 * PURPOSE:
 *   Define an owned toolkit-neutral view model with stable identity, hierarchy,
 *   visibility, enablement and revisioned properties.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_VIEW_MODEL_H
#define UMICOM_UI_VIEW_MODEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/property.h"
#include "umicom/ui/types.h"

#ifdef __cplusplus
extern "C" {
#endif


#define UMI_UI_VIEW_CHILD_MAX 64U

/**
 * Represent the ui view snapshot data shared with callers of this public contract.
 */
typedef struct UmiUiViewSnapshot {
    char view_id[UMI_UI_ID_CAPACITY];
    char view_type[UMI_UI_ID_CAPACITY];
    char parent_id[UMI_UI_ID_CAPACITY];
    UmiUiRole role;
    int visible;
    int enabled;
    uint64_t revision;
    size_t child_count;
} UmiUiViewSnapshot;

/**
 * Represent the ui view model data shared with callers of this public contract.
 */
typedef struct UmiUiViewModel UmiUiViewModel;

/**
 * Initialise ui view model from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_ui_view_model_create(const char *view_id, const char *view_type,
                                   UmiUiRole role, UmiUiViewModel **out_view);
/**
 * Release or reset state held by ui view model so the same storage can be reused safely.
 */
void umi_ui_view_model_destroy(UmiUiViewModel *view);
/**
 * Provide the ui view model set parent operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_set_parent(UmiUiViewModel *view, const char *parent_id);
/**
 * Provide the ui view model add child operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_add_child(UmiUiViewModel *view, const char *child_id);
/**
 * Provide the ui view model remove child operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_remove_child(UmiUiViewModel *view, const char *child_id);
/**
 * Provide the ui view model set visible operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_set_visible(UmiUiViewModel *view, int visible);
/**
 * Provide the ui view model set enabled operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_set_enabled(UmiUiViewModel *view, int enabled);
/**
 * Provide the ui view model set property operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_set_property(UmiUiViewModel *view, const char *key,
                                         const UmiUiValue *value);
/**
 * Provide the ui view model get property operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_get_property(const UmiUiViewModel *view, const char *key,
                                         UmiUiValue *out_value);
/**
 * Provide the ui view model snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_view_model_snapshot(const UmiUiViewModel *view,
                                     UmiUiViewSnapshot *out_snapshot);
/**
 * Find ui view model child while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_ui_view_model_child_at(const UmiUiViewModel *view, size_t index,
                                     char *out_child_id, size_t capacity);
/**
 * Provide the ui view model properties operation used by this module and its client
 * applications.
 */
UmiUiPropertyBag *umi_ui_view_model_properties(UmiUiViewModel *view);

#ifdef __cplusplus
}
#endif

#endif
