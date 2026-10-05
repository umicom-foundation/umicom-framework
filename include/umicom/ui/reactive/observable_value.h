/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/observable_value.h
 *
 * PURPOSE:
 *   Hold a revisioned value and dirty state for reactive propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_OBSERVABLE_VALUE_H
#define UMICOM_UI_REACTIVE_OBSERVABLE_VALUE_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive observable value data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveObservableValue {
    UmiUiValue value;
    uint64_t revision;
    bool dirty;
} UmiUiReactiveObservableValue;
/**
 * Initialise ui reactive observable value from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_observable_value_init(UmiUiReactiveObservableValue *item);
/**
 * Check that ui reactive observable value satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_observable_value_valid(const UmiUiReactiveObservableValue *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_observable_value_archive_encode(const UmiUiReactiveObservableValue *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_observable_value_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveObservableValue *value);

#ifdef __cplusplus
}
#endif
#endif
