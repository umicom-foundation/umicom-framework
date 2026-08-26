/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/command_parameter.h
 *
 * PURPOSE:
 *   Represent a revisioned command parameter value.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMMAND_PARAMETER_H
#define UMICOM_UI_REACTIVE_COMMAND_PARAMETER_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveCommandParameter {
    char name[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiValue value;
    uint64_t revision;
} UmiUiReactiveCommandParameter;
void umi_ui_reactive_command_parameter_init(UmiUiReactiveCommandParameter *item);
int umi_ui_reactive_command_parameter_valid(const UmiUiReactiveCommandParameter *item);
#ifdef __cplusplus
}
#endif
#endif
