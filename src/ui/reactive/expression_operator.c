/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_operator.c
 *
 * PURPOSE:
 *   Implement supported comparison and logical operators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_operator.h"
#include <string.h>

/* Initialise the expression operator contract to deterministic zero/default state. */
void umi_ui_reactive_expression_operator_init(UmiUiReactiveExpressionOperator *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_operator_valid(const UmiUiReactiveExpressionOperator *item) {
    return item != NULL;
}
