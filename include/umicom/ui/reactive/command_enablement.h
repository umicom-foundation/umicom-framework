/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/command_enablement.h
 *
 * PURPOSE:
 *   Represent command enablement evidence from a state expression.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMMAND_ENABLEMENT_H
#define UMICOM_UI_REACTIVE_COMMAND_ENABLEMENT_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive command enablement data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveCommandEnablement {
    char command_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool enabled;
    uint64_t evaluation_revision;
} UmiUiReactiveCommandEnablement;
/**
 * Initialise ui reactive command enablement from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_command_enablement_init(UmiUiReactiveCommandEnablement *item);
/**
 * Check that ui reactive command enablement satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_command_enablement_valid(const UmiUiReactiveCommandEnablement *item);
#ifdef __cplusplus
}
#endif
#endif
