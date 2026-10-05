/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/built_in_converters.h
 *
 * PURPOSE:
 *   Provide deterministic scalar conversion helpers used by declarative bindings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BUILT_IN_CONVERTERS_H
#define UMICOM_UI_REACTIVE_BUILT_IN_CONVERTERS_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive built in converters data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBuiltInConverters {
    bool allow_lossy_numeric;
    bool trim_strings;
} UmiUiReactiveBuiltInConverters;
/**
 * Initialise ui reactive built in converters from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_built_in_converters_init(UmiUiReactiveBuiltInConverters *item);
/**
 * Check that ui reactive built in converters satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_built_in_converters_valid(const UmiUiReactiveBuiltInConverters *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_built_in_converters_archive_encode(const UmiUiReactiveBuiltInConverters *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_built_in_converters_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveBuiltInConverters *value);

#ifdef __cplusplus
}
#endif
#endif
