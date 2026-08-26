/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_expression_program.c
 *
 * PURPOSE:
 *   Exercise the expression program reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_program.h"
int main(void) { UmiUiReactiveExpressionProgram item; umi_ui_reactive_expression_program_init(&item); return umi_ui_reactive_expression_program_valid(&item) ? 0 : 1; }
