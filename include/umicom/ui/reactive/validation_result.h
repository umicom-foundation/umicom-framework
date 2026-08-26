/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/validation_result.h
 *
 * PURPOSE:
 *   Represent deterministic validation outcome and user-facing message.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VALIDATION_RESULT_H
#define UMICOM_UI_REACTIVE_VALIDATION_RESULT_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveValidationResult {
    char rule_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool valid;
    UmiUiReactiveValidationSeverity severity;
    char message[UMI_UI_REACTIVE_TEXT_CAPACITY];
} UmiUiReactiveValidationResult;
void umi_ui_reactive_validation_result_init(UmiUiReactiveValidationResult *item);
int umi_ui_reactive_validation_result_valid(const UmiUiReactiveValidationResult *item);
#ifdef __cplusplus
}
#endif
#endif
