/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/context_binding.h
 *
 * PURPOSE:
 *   Synchronise typed context channels with declarative UI properties.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CONTEXT_BINDING_H
#define UMICOM_UI_REACTIVE_CONTEXT_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveContextBinding {
    char channel_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char property_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    bool publish;
    bool subscribe;
} UmiUiReactiveContextBinding;
void umi_ui_reactive_context_binding_init(UmiUiReactiveContextBinding *item);
int umi_ui_reactive_context_binding_valid(const UmiUiReactiveContextBinding *item);
#ifdef __cplusplus
}
#endif
#endif
