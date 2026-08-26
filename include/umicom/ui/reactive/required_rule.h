/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/required_rule.h
 *
 * PURPOSE:
 *   Bind form required-state to declarative state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_REQUIRED_RULE_H
#define UMICOM_UI_REACTIVE_REQUIRED_RULE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveRequiredRule {
    char target_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char expression[UMI_UI_REACTIVE_TEXT_CAPACITY];
    bool required;
} UmiUiReactiveRequiredRule;
void umi_ui_reactive_required_rule_init(UmiUiReactiveRequiredRule *item);
int umi_ui_reactive_required_rule_valid(const UmiUiReactiveRequiredRule *item);
#ifdef __cplusplus
}
#endif
#endif
