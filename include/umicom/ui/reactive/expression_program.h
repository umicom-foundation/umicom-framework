/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_program.h
 *
 * PURPOSE:
 *   Store a bounded reverse-polish expression program.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_PROGRAM_H
#define UMICOM_UI_REACTIVE_EXPRESSION_PROGRAM_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveExpressionProgram {
    int opcodes[UMI_UI_REACTIVE_MAX_TOKENS];
    double numbers[UMI_UI_REACTIVE_MAX_TOKENS];
    size_t count;
} UmiUiReactiveExpressionProgram;
void umi_ui_reactive_expression_program_init(UmiUiReactiveExpressionProgram *item);
int umi_ui_reactive_expression_program_valid(const UmiUiReactiveExpressionProgram *item);
#ifdef __cplusplus
}
#endif
#endif
