/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/built_in_converters.h
 *
 * PURPOSE:
 *   Provide deterministic scalar conversion helpers used by declarative bindings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BUILT_IN_CONVERTERS_H
#define UMICOM_UI_REACTIVE_BUILT_IN_CONVERTERS_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveBuiltInConverters {
    bool allow_lossy_numeric;
    bool trim_strings;
} UmiUiReactiveBuiltInConverters;
void umi_ui_reactive_built_in_converters_init(UmiUiReactiveBuiltInConverters *item);
int umi_ui_reactive_built_in_converters_valid(const UmiUiReactiveBuiltInConverters *item);
#ifdef __cplusplus
}
#endif
#endif
