/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/readonly_rule.h
 *
 * PURPOSE:
 *   Bind editor read-only state to declarative state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_READONLY_RULE_H
#define UMICOM_UI_REACTIVE_READONLY_RULE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveReadonlyRule {
    char target_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char expression[UMI_UI_REACTIVE_TEXT_CAPACITY];
    bool read_only;
} UmiUiReactiveReadonlyRule;
void umi_ui_reactive_readonly_rule_init(UmiUiReactiveReadonlyRule *item);
int umi_ui_reactive_readonly_rule_valid(const UmiUiReactiveReadonlyRule *item);
#ifdef __cplusplus
}
#endif
#endif
