/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/validation_group.h
 *
 * PURPOSE:
 *   Aggregate validation results for one form, object or editing transaction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_VALIDATION_GROUP_H
#define UMICOM_UI_REACTIVE_VALIDATION_GROUP_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive validation group data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveValidationGroup {
    size_t total;
    size_t invalid;
    size_t warnings;
    bool blocking;
} UmiUiReactiveValidationGroup;
/**
 * Initialise ui reactive validation group from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_validation_group_init(UmiUiReactiveValidationGroup *item);
/**
 * Check that ui reactive validation group satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_validation_group_valid(const UmiUiReactiveValidationGroup *item);
#ifdef __cplusplus
}
#endif
#endif
