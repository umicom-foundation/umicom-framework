/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/view_model.h
 *
 * PURPOSE:
 *   Define an owned toolkit-neutral view model with stable identity, hierarchy,
 *   visibility, enablement and revisioned properties.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiUiViewModel UmiUiViewModel;

UmiStatus umi_ui_view_model_create(const char *view_id, const char *view_type,
                                   UmiUiRole role, UmiUiViewModel **out_view);
void umi_ui_view_model_destroy(UmiUiViewModel *view);
UmiStatus umi_ui_view_model_set_parent(UmiUiViewModel *view, const char *parent_id);
UmiStatus umi_ui_view_model_add_child(UmiUiViewModel *view, const char *child_id);
UmiStatus umi_ui_view_model_remove_child(UmiUiViewModel *view, const char *child_id);
UmiStatus umi_ui_view_model_set_visible(UmiUiViewModel *view, int visible);
UmiStatus umi_ui_view_model_set_enabled(UmiUiViewModel *view, int enabled);
UmiStatus umi_ui_view_model_set_property(UmiUiViewModel *view, const char *key,
                                         const UmiUiValue *value);
UmiStatus umi_ui_view_model_get_property(const UmiUiViewModel *view, const char *key,
                                         UmiUiValue *out_value);
UmiStatus umi_ui_view_model_snapshot(const UmiUiViewModel *view,
                                     UmiUiViewSnapshot *out_snapshot);
UmiStatus umi_ui_view_model_child_at(const UmiUiViewModel *view, size_t index,
                                     char *out_child_id, size_t capacity);
UmiUiPropertyBag *umi_ui_view_model_properties(UmiUiViewModel *view);

#ifdef __cplusplus
}
#endif

#endif
