/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/command_parameter.h
 *
 * PURPOSE:
 *   Represent a revisioned command parameter value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMMAND_PARAMETER_H
#define UMICOM_UI_REACTIVE_COMMAND_PARAMETER_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive command parameter data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveCommandParameter {
    char name[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiValue value;
    uint64_t revision;
} UmiUiReactiveCommandParameter;
/**
 * Initialise ui reactive command parameter from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_command_parameter_init(UmiUiReactiveCommandParameter *item);
/**
 * Check that ui reactive command parameter satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_command_parameter_valid(const UmiUiReactiveCommandParameter *item);
#ifdef __cplusplus
}
#endif
#endif
