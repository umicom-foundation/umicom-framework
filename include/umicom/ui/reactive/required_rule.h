/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/required_rule.h
 *
 * PURPOSE:
 *   Bind form required-state to declarative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_REQUIRED_RULE_H
#define UMICOM_UI_REACTIVE_REQUIRED_RULE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive required rule data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveRequiredRule {
    char target_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char expression[UMI_UI_REACTIVE_TEXT_CAPACITY];
    bool required;
} UmiUiReactiveRequiredRule;
/**
 * Initialise ui reactive required rule from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_required_rule_init(UmiUiReactiveRequiredRule *item);
/**
 * Check that ui reactive required rule satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_required_rule_valid(const UmiUiReactiveRequiredRule *item);
#ifdef __cplusplus
}
#endif
#endif
