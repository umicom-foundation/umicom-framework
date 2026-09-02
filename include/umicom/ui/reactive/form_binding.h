/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/form_binding.h
 *
 * PURPOSE:
 *   Describe form-level model binding and commit policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_FORM_BINDING_H
#define UMICOM_UI_REACTIVE_FORM_BINDING_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive form binding data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveFormBinding {
    char form_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char model_prefix[UMI_UI_REACTIVE_PATH_CAPACITY];
    UmiUiReactiveUpdateTrigger trigger;
    bool validate_before_commit;
} UmiUiReactiveFormBinding;
/**
 * Initialise ui reactive form binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_form_binding_init(UmiUiReactiveFormBinding *item);
/**
 * Check that ui reactive form binding satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_form_binding_valid(const UmiUiReactiveFormBinding *item);
#ifdef __cplusplus
}
#endif
#endif
