/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_evaluator.c
 *
 * PURPOSE:
 *   Implement deterministic expression evaluation outcome.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_evaluator.h"
#include <string.h>

/* Initialise the expression evaluator contract to deterministic zero/default state. */
void umi_ui_reactive_expression_evaluator_init(UmiUiReactiveExpressionEvaluator *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_evaluator_valid(const UmiUiReactiveExpressionEvaluator *item) {
    return item != NULL;
}
