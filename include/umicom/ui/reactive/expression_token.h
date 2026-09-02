/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_token.h
 *
 * PURPOSE:
 *   Represent a compact token used by renderer-neutral state expressions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_TOKEN_H
#define UMICOM_UI_REACTIVE_EXPRESSION_TOKEN_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive expression token data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveExpressionToken {
    int kind;
    char text[64];
    double number;
} UmiUiReactiveExpressionToken;
/**
 * Initialise ui reactive expression token from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_expression_token_init(UmiUiReactiveExpressionToken *item);
/**
 * Check that ui reactive expression token satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_expression_token_valid(const UmiUiReactiveExpressionToken *item);
#ifdef __cplusplus
}
#endif
#endif
