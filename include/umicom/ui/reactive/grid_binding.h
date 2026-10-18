/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/grid_binding.h
 *
 * PURPOSE:
 *   Describe enterprise grid provider/query/selection binding paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_GRID_BINDING_H
#define UMICOM_UI_REACTIVE_GRID_BINDING_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive grid binding data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveGridBinding {
    char grid_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char provider_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char query_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char selection_path[UMI_UI_REACTIVE_PATH_CAPACITY];
} UmiUiReactiveGridBinding;
/**
 * Initialise ui reactive grid binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_grid_binding_init(UmiUiReactiveGridBinding *item);
/**
 * Check that ui reactive grid binding satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_grid_binding_valid(const UmiUiReactiveGridBinding *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_grid_binding_archive_encode(const UmiUiReactiveGridBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_grid_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveGridBinding *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiUiReactiveGridBinding object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_ui_reactive_grid_binding_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiUiReactiveGridBinding *workspace, UmiUiReactiveGridBinding *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_reactive_grid_binding_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiReactiveGridBinding *expected, UmiUiReactiveGridBinding *value);


#ifdef __cplusplus
}
#endif
#endif
