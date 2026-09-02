/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/built_in_converters.h
 *
 * PURPOSE:
 *   Provide deterministic scalar conversion helpers used by declarative bindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BUILT_IN_CONVERTERS_H
#define UMICOM_UI_REACTIVE_BUILT_IN_CONVERTERS_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive built in converters data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBuiltInConverters {
    bool allow_lossy_numeric;
    bool trim_strings;
} UmiUiReactiveBuiltInConverters;
/**
 * Initialise ui reactive built in converters from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_built_in_converters_init(UmiUiReactiveBuiltInConverters *item);
/**
 * Check that ui reactive built in converters satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_built_in_converters_valid(const UmiUiReactiveBuiltInConverters *item);
#ifdef __cplusplus
}
#endif
#endif
