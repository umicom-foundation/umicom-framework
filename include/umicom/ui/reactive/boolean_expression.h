/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/boolean_expression.h
 *
 * PURPOSE:
 *   Represent a boolean expression result with source revision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BOOLEAN_EXPRESSION_H
#define UMICOM_UI_REACTIVE_BOOLEAN_EXPRESSION_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveBooleanExpression {
    bool value;
    uint64_t source_revision;
} UmiUiReactiveBooleanExpression;
void umi_ui_reactive_boolean_expression_init(UmiUiReactiveBooleanExpression *item);
int umi_ui_reactive_boolean_expression_valid(const UmiUiReactiveBooleanExpression *item);
#ifdef __cplusplus
}
#endif
#endif
