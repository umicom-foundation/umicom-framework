/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/boolean_expression.c
 *
 * PURPOSE:
 *   Implement a boolean expression result with source revision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/boolean_expression.h"
#include <string.h>

/* Initialise the boolean expression contract to deterministic zero/default state. */
void umi_ui_reactive_boolean_expression_init(UmiUiReactiveBooleanExpression *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_boolean_expression_valid(const UmiUiReactiveBooleanExpression *item) {
    return item != NULL;
}
