/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/converter.h
 *
 * PURPOSE:
 *   Describe a named value converter with forward and reverse availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CONVERTER_H
#define UMICOM_UI_REACTIVE_CONVERTER_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive converter data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveConverter {
    char converter_id[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiValueKind source_kind;
    UmiUiValueKind target_kind;
    bool supports_reverse;
} UmiUiReactiveConverter;
/**
 * Initialise ui reactive converter from caller-provided values so later operations receive
 * a known state.
 */
void umi_ui_reactive_converter_init(UmiUiReactiveConverter *item);
/**
 * Check that ui reactive converter satisfies its contract before another service relies on
 * it.
 */
int umi_ui_reactive_converter_valid(const UmiUiReactiveConverter *item);
#ifdef __cplusplus
}
#endif
#endif
