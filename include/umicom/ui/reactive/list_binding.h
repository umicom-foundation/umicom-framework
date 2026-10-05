/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/list_binding.h
 *
 * PURPOSE:
 *   Describe list data-path and selection-path binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_LIST_BINDING_H
#define UMICOM_UI_REACTIVE_LIST_BINDING_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive list binding data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveListBinding {
    char surface_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char items_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char selection_path[UMI_UI_REACTIVE_PATH_CAPACITY];
} UmiUiReactiveListBinding;
/**
 * Initialise ui reactive list binding from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_list_binding_init(UmiUiReactiveListBinding *item);
/**
 * Check that ui reactive list binding satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_list_binding_valid(const UmiUiReactiveListBinding *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_list_binding_archive_encode(const UmiUiReactiveListBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_list_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveListBinding *value);

#ifdef __cplusplus
}
#endif
#endif
