/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/enablement_rule.h
 *
 * PURPOSE:
 *   Bind component enablement to declarative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_ENABLEMENT_RULE_H
#define UMICOM_UI_REACTIVE_ENABLEMENT_RULE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive enablement rule data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveEnablementRule {
    char target_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char expression[UMI_UI_REACTIVE_TEXT_CAPACITY];
    bool enabled;
} UmiUiReactiveEnablementRule;
/**
 * Initialise ui reactive enablement rule from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_enablement_rule_init(UmiUiReactiveEnablementRule *item);
/**
 * Check that ui reactive enablement rule satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_enablement_rule_valid(const UmiUiReactiveEnablementRule *item);
#ifdef __cplusplus
}
#endif
#endif
