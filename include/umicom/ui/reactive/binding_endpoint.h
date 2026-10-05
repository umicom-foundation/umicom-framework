/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_endpoint.h
 *
 * PURPOSE:
 *   Describe a view/property endpoint without owning the target object.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_ENDPOINT_H
#define UMICOM_UI_REACTIVE_BINDING_ENDPOINT_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive binding endpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBindingEndpoint {
    char view_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char property_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    bool writable;
} UmiUiReactiveBindingEndpoint;
/**
 * Initialise ui reactive binding endpoint from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_binding_endpoint_init(UmiUiReactiveBindingEndpoint *item);
/**
 * Check that ui reactive binding endpoint satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_binding_endpoint_valid(const UmiUiReactiveBindingEndpoint *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_binding_endpoint_archive_encode(const UmiUiReactiveBindingEndpoint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_binding_endpoint_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveBindingEndpoint *value);

#ifdef __cplusplus
}
#endif
#endif
