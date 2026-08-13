/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/command_view.h
 *
 * PURPOSE:
 *   Define a toolkit-neutral convention for placing executable workbench
 *   actions inside a reusable view model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_COMMAND_VIEW_H
#define UMICOM_UI_COMMAND_VIEW_H

#include <stddef.h>
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_COMMAND_VIEW_ACTION_MAX 16U
#define UMI_UI_COMMAND_VIEW_ACTION_COUNT_KEY "command-view.action-count"

typedef struct UmiUiCommandViewAction {
    char action_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    char tooltip[UMI_UI_DESCRIPTION_CAPACITY];
    int enabled;
} UmiUiCommandViewAction;

UmiStatus umi_ui_command_view_set_action(UmiUiViewModel *view,
                                         size_t index,
                                         const UmiUiCommandViewAction *action);
UmiStatus umi_ui_command_view_action_at(const UmiUiViewModel *view,
                                        size_t index,
                                        UmiUiCommandViewAction *out_action);
int umi_ui_command_view_property_is_reserved(const char *property_key);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_UI_COMMAND_VIEW_H */
