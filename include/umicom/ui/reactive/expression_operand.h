/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_operand.h
 *
 * PURPOSE:
 *   Represent scalar expression operands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_OPERAND_H
#define UMICOM_UI_REACTIVE_EXPRESSION_OPERAND_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive expression operand data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveExpressionOperand {
    UmiUiValue value;
    bool resolved;
} UmiUiReactiveExpressionOperand;
/**
 * Initialise ui reactive expression operand from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_expression_operand_init(UmiUiReactiveExpressionOperand *item);
/**
 * Check that ui reactive expression operand satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_expression_operand_valid(const UmiUiReactiveExpressionOperand *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_expression_operand_archive_encode(const UmiUiReactiveExpressionOperand *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_expression_operand_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveExpressionOperand *value);

#ifdef __cplusplus
}
#endif
#endif
