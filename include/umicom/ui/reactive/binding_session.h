/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_session.h
 *
 * PURPOSE:
 *   Track binding activation, revision and propagation counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_SESSION_H
#define UMICOM_UI_REACTIVE_BINDING_SESSION_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveBindingSession {
    char session_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool active;
    uint64_t revision;
    size_t propagations;
} UmiUiReactiveBindingSession;
void umi_ui_reactive_binding_session_init(UmiUiReactiveBindingSession *item);
int umi_ui_reactive_binding_session_valid(const UmiUiReactiveBindingSession *item);
#ifdef __cplusplus
}
#endif
#endif
