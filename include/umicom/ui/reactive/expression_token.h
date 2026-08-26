/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_token.h
 *
 * PURPOSE:
 *   Represent a compact token used by renderer-neutral state expressions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_TOKEN_H
#define UMICOM_UI_REACTIVE_EXPRESSION_TOKEN_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveExpressionToken {
    int kind;
    char text[64];
    double number;
} UmiUiReactiveExpressionToken;
void umi_ui_reactive_expression_token_init(UmiUiReactiveExpressionToken *item);
int umi_ui_reactive_expression_token_valid(const UmiUiReactiveExpressionToken *item);
#ifdef __cplusplus
}
#endif
#endif
