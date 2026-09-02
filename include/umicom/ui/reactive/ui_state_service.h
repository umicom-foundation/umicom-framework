/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/ui_state_service.h
 *
 * PURPOSE:
 *   Represent aggregate readiness of binding, validation and state-graph services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_UI_STATE_SERVICE_H
#define UMICOM_UI_REACTIVE_UI_STATE_SERVICE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive ui state service data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveUiStateService {
    bool bindings_ready;
    bool validation_ready;
    bool graph_ready;
    bool scheduler_ready;
    uint64_t revision;
} UmiUiReactiveUiStateService;
/**
 * Initialise ui reactive ui state service from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_ui_state_service_init(UmiUiReactiveUiStateService *item);
/**
 * Check that ui reactive ui state service satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_ui_state_service_valid(const UmiUiReactiveUiStateService *item);
#ifdef __cplusplus
}
#endif
#endif
