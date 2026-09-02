/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/readonly_rule.h
 *
 * PURPOSE:
 *   Bind editor read-only state to declarative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_READONLY_RULE_H
#define UMICOM_UI_REACTIVE_READONLY_RULE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive readonly rule data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveReadonlyRule {
    char target_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char expression[UMI_UI_REACTIVE_TEXT_CAPACITY];
    bool read_only;
} UmiUiReactiveReadonlyRule;
/**
 * Initialise ui reactive readonly rule from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_readonly_rule_init(UmiUiReactiveReadonlyRule *item);
/**
 * Check that ui reactive readonly rule satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_readonly_rule_valid(const UmiUiReactiveReadonlyRule *item);
#ifdef __cplusplus
}
#endif
#endif
