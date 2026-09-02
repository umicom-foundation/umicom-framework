/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_program.c
 *
 * PURPOSE:
 *   Implement a bounded reverse-polish expression program.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_program.h"
#include <string.h>

/* Initialise the expression program contract to deterministic zero/default state. */
void umi_ui_reactive_expression_program_init(UmiUiReactiveExpressionProgram *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_program_valid(const UmiUiReactiveExpressionProgram *item) {
    return item != NULL;
}
