/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_descriptor.h
 *
 * PURPOSE:
 *   Describe a declarative binding between source and target endpoints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_DESCRIPTOR_H
#define UMICOM_UI_REACTIVE_BINDING_DESCRIPTOR_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive binding descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveBindingDescriptor {
    char binding_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char source_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    char target_path[UMI_UI_REACTIVE_PATH_CAPACITY];
    UmiUiReactiveBindingDirection direction;
    UmiUiReactiveUpdateTrigger trigger;
    bool enabled;
} UmiUiReactiveBindingDescriptor;
/**
 * Initialise ui reactive binding descriptor from caller-provided values so later
 * operations receive a known state.
 */
void umi_ui_reactive_binding_descriptor_init(UmiUiReactiveBindingDescriptor *item);
/**
 * Check that ui reactive binding descriptor satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_binding_descriptor_valid(const UmiUiReactiveBindingDescriptor *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_binding_descriptor_archive_encode(const UmiUiReactiveBindingDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_binding_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveBindingDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
