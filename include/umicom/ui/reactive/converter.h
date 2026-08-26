/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/converter.h
 *
 * PURPOSE:
 *   Describe a named value converter with forward and reverse availability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CONVERTER_H
#define UMICOM_UI_REACTIVE_CONVERTER_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveConverter {
    char converter_id[UMI_UI_REACTIVE_ID_CAPACITY];
    UmiUiValueKind source_kind;
    UmiUiValueKind target_kind;
    bool supports_reverse;
} UmiUiReactiveConverter;
void umi_ui_reactive_converter_init(UmiUiReactiveConverter *item);
int umi_ui_reactive_converter_valid(const UmiUiReactiveConverter *item);
#ifdef __cplusplus
}
#endif
#endif
