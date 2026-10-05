/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/expression_evaluator.h
 *
 * PURPOSE:
 *   Store deterministic expression evaluation outcome.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_EXPRESSION_EVALUATOR_H
#define UMICOM_UI_REACTIVE_EXPRESSION_EVALUATOR_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive expression evaluator data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveExpressionEvaluator {
    UmiUiValue result;
    size_t instructions_executed;
    bool success;
} UmiUiReactiveExpressionEvaluator;
/**
 * Initialise ui reactive expression evaluator from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_expression_evaluator_init(UmiUiReactiveExpressionEvaluator *item);
/**
 * Check that ui reactive expression evaluator satisfies its contract before another
 * service relies on it.
 */
int umi_ui_reactive_expression_evaluator_valid(const UmiUiReactiveExpressionEvaluator *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_expression_evaluator_archive_encode(const UmiUiReactiveExpressionEvaluator *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_expression_evaluator_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveExpressionEvaluator *value);

#ifdef __cplusplus
}
#endif
#endif
