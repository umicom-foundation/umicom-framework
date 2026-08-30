/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/validator.h
 *
 * PURPOSE:
 *   Describe a named validator and the value kind it accepts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VALIDATOR_H
#define UMICOM_UI_REACTIVE_VALIDATOR_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveValidator {
    char validator_id[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiValueKind value_kind;
    UmiUiReactiveValidationSeverity severity;
    bool enabled;
} UmiUiReactiveValidator;
void umi_ui_reactive_validator_init(UmiUiReactiveValidator *item);
int umi_ui_reactive_validator_valid(const UmiUiReactiveValidator *item);
#ifdef __cplusplus
}
#endif
#endif
