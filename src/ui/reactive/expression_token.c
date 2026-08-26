/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/expression_token.c
 *
 * PURPOSE:
 *   Implement a compact token used by renderer-neutral state expressions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/expression_token.h"
#include <string.h>

/* Initialise the expression token contract to deterministic zero/default state. */
void umi_ui_reactive_expression_token_init(UmiUiReactiveExpressionToken *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_expression_token_valid(const UmiUiReactiveExpressionToken *item) {
    return item != NULL;
}
