/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_session.h
 *
 * PURPOSE:
 *   Track binding activation, revision and propagation counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_SESSION_H
#define UMICOM_UI_REACTIVE_BINDING_SESSION_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive binding session data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBindingSession {
    char session_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool active;
    uint64_t revision;
    size_t propagations;
} UmiUiReactiveBindingSession;
/**
 * Initialise ui reactive binding session from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_binding_session_init(UmiUiReactiveBindingSession *item);
/**
 * Check that ui reactive binding session satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_binding_session_valid(const UmiUiReactiveBindingSession *item);
#ifdef __cplusplus
}
#endif
#endif
