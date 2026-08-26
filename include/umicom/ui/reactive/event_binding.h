/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/event_binding.h
 *
 * PURPOSE:
 *   Route a semantic UI event to a command or state action.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EVENT_BINDING_H
#define UMICOM_UI_REACTIVE_EVENT_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveEventBinding {
    char source_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char event_name[UMI_UI_REACTIVE_ID_CAPACITY];
    char action_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool enabled;
} UmiUiReactiveEventBinding;
void umi_ui_reactive_event_binding_init(UmiUiReactiveEventBinding *item);
int umi_ui_reactive_event_binding_valid(const UmiUiReactiveEventBinding *item);
#ifdef __cplusplus
}
#endif
#endif
