/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_operand.c
 *
 * PURPOSE:
 *   Implement scalar expression operands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_operand.h"
#include <string.h>

/* Initialise the expression operand contract to deterministic zero/default state. */
void umi_ui_reactive_expression_operand_init(UmiUiReactiveExpressionOperand *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_operand_valid(const UmiUiReactiveExpressionOperand *item) {
    return item != NULL;
}
