/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/inspector_binding.h
 *
 * PURPOSE:
 *   Describe inspector subject and editing binding paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_INSPECTOR_BINDING_H
#define UMICOM_UI_REACTIVE_INSPECTOR_BINDING_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive inspector binding data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveInspectorBinding {
    char inspector_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char subject_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char edit_path[UMI_UI_REACTIVE_PATH_CAPACITY];
} UmiUiReactiveInspectorBinding;
/**
 * Initialise ui reactive inspector binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_inspector_binding_init(UmiUiReactiveInspectorBinding *item);
/**
 * Check that ui reactive inspector binding satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_inspector_binding_valid(const UmiUiReactiveInspectorBinding *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_inspector_binding_archive_encode(const UmiUiReactiveInspectorBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_inspector_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveInspectorBinding *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_reactive_inspector_binding_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiReactiveInspectorBinding *expected, UmiUiReactiveInspectorBinding *value);


#ifdef __cplusplus
}
#endif
#endif
