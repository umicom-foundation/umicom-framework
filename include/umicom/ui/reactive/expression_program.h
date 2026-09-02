/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_program.h
 *
 * PURPOSE:
 *   Store a bounded reverse-polish expression program.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_PROGRAM_H
#define UMICOM_UI_REACTIVE_EXPRESSION_PROGRAM_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive expression program data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveExpressionProgram {
    int opcodes[UMI_UI_REACTIVE_MAX_TOKENS];
    double numbers[UMI_UI_REACTIVE_MAX_TOKENS];
    size_t count;
} UmiUiReactiveExpressionProgram;
/**
 * Initialise ui reactive expression program from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_expression_program_init(UmiUiReactiveExpressionProgram *item);
/**
 * Check that ui reactive expression program satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_expression_program_valid(const UmiUiReactiveExpressionProgram *item);
#ifdef __cplusplus
}
#endif
#endif
