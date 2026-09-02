/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/computed_value.h
 *
 * PURPOSE:
 *   Describe derived state and its last computed revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_COMPUTED_VALUE_H
#define UMICOM_UI_REACTIVE_COMPUTED_VALUE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive computed value data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveComputedValue {
    char computed_id[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiValue value;
    uint64_t revision;
    bool valid;
} UmiUiReactiveComputedValue;
/**
 * Initialise ui reactive computed value from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_computed_value_init(UmiUiReactiveComputedValue *item);
/**
 * Check that ui reactive computed value satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_computed_value_valid(const UmiUiReactiveComputedValue *item);
#ifdef __cplusplus
}
#endif
#endif
