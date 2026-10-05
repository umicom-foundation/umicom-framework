/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/boolean_expression.h
 *
 * PURPOSE:
 *   Represent a boolean expression result with source revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BOOLEAN_EXPRESSION_H
#define UMICOM_UI_REACTIVE_BOOLEAN_EXPRESSION_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive boolean expression data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBooleanExpression {
    bool value;
    uint64_t source_revision;
} UmiUiReactiveBooleanExpression;
/**
 * Initialise ui reactive boolean expression from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_boolean_expression_init(UmiUiReactiveBooleanExpression *item);
/**
 * Check that ui reactive boolean expression satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_boolean_expression_valid(const UmiUiReactiveBooleanExpression *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_boolean_expression_archive_encode(const UmiUiReactiveBooleanExpression *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_boolean_expression_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveBooleanExpression *value);

#ifdef __cplusplus
}
#endif
#endif
