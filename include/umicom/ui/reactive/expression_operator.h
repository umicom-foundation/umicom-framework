/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_operator.h
 *
 * PURPOSE:
 *   Represent supported comparison and logical operators.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_OPERATOR_H
#define UMICOM_UI_REACTIVE_EXPRESSION_OPERATOR_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive expression operator data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveExpressionOperator {
    int op;
    int precedence;
} UmiUiReactiveExpressionOperator;
/**
 * Initialise ui reactive expression operator from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_expression_operator_init(UmiUiReactiveExpressionOperator *item);
/**
 * Check that ui reactive expression operator satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_expression_operator_valid(const UmiUiReactiveExpressionOperator *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_expression_operator_archive_encode(const UmiUiReactiveExpressionOperator *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_expression_operator_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveExpressionOperator *value);

#ifdef __cplusplus
}
#endif
#endif
