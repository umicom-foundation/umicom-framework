/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/enablement_rule.h
 *
 * PURPOSE:
 *   Bind component enablement to declarative state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_ENABLEMENT_RULE_H
#define UMICOM_UI_REACTIVE_ENABLEMENT_RULE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveEnablementRule {
    char target_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char expression[UMI_UI_REACTIVE_TEXT_CAPACITY];
    bool enabled;
} UmiUiReactiveEnablementRule;
void umi_ui_reactive_enablement_rule_init(UmiUiReactiveEnablementRule *item);
int umi_ui_reactive_enablement_rule_valid(const UmiUiReactiveEnablementRule *item);
#ifdef __cplusplus
}
#endif
#endif
