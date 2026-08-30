/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/validation_rule.h
 *
 * PURPOSE:
 *   Represent common required/range/length validation constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VALIDATION_RULE_H
#define UMICOM_UI_REACTIVE_VALIDATION_RULE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveValidationRule {
    char rule_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool required;
    double minimum;
    double maximum;
    size_t min_length;
    size_t max_length;
} UmiUiReactiveValidationRule;
void umi_ui_reactive_validation_rule_init(UmiUiReactiveValidationRule *item);
int umi_ui_reactive_validation_rule_valid(const UmiUiReactiveValidationRule *item);
#ifdef __cplusplus
}
#endif
#endif
