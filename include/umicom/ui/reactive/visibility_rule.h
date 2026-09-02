/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/visibility_rule.h
 *
 * PURPOSE:
 *   Bind component visibility to declarative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VISIBILITY_RULE_H
#define UMICOM_UI_REACTIVE_VISIBILITY_RULE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive visibility rule data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveVisibilityRule {
    char target_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char expression[UMI_UI_REACTIVE_TEXT_CAPACITY];
    bool visible;
} UmiUiReactiveVisibilityRule;
/**
 * Initialise ui reactive visibility rule from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_visibility_rule_init(UmiUiReactiveVisibilityRule *item);
/**
 * Check that ui reactive visibility rule satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_visibility_rule_valid(const UmiUiReactiveVisibilityRule *item);
#ifdef __cplusplus
}
#endif
#endif
