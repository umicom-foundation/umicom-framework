/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_scheduler.h
 *
 * PURPOSE:
 *   Track pending propagation work and deterministic dispatch generation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_SCHEDULER_H
#define UMICOM_UI_REACTIVE_BINDING_SCHEDULER_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveBindingScheduler {
    size_t pending;
    uint64_t generation;
    bool suspended;
} UmiUiReactiveBindingScheduler;
void umi_ui_reactive_binding_scheduler_init(UmiUiReactiveBindingScheduler *item);
int umi_ui_reactive_binding_scheduler_valid(const UmiUiReactiveBindingScheduler *item);
#ifdef __cplusplus
}
#endif
#endif
