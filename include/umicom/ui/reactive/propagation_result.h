/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/propagation_result.h
 *
 * PURPOSE:
 *   Summarise propagated, skipped and failed binding operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_PROPAGATION_RESULT_H
#define UMICOM_UI_REACTIVE_PROPAGATION_RESULT_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive propagation result data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactivePropagationResult {
    size_t propagated;
    size_t skipped;
    size_t failed;
    uint64_t generation;
} UmiUiReactivePropagationResult;
/**
 * Initialise ui reactive propagation result from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_propagation_result_init(UmiUiReactivePropagationResult *item);
/**
 * Check that ui reactive propagation result satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_propagation_result_valid(const UmiUiReactivePropagationResult *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_propagation_result_archive_encode(const UmiUiReactivePropagationResult *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_propagation_result_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactivePropagationResult *value);

#ifdef __cplusplus
}
#endif
#endif
