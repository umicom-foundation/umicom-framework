/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/binding_direction.h
 *
 * PURPOSE:
 *   Describe binding direction and update trigger policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_BINDING_DIRECTION_H
#define UMICOM_UI_REACTIVE_BINDING_DIRECTION_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive binding direction policy data shared with callers of this
 * public contract.
 */
typedef struct UmiUiReactiveBindingDirectionPolicy {
    UmiUiReactiveBindingDirection direction;
    UmiUiReactiveUpdateTrigger trigger;
    bool propagate_initial;
} UmiUiReactiveBindingDirectionPolicy;
/**
 * Initialise ui reactive binding direction from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_binding_direction_init(UmiUiReactiveBindingDirectionPolicy *item);
/**
 * Check that ui reactive binding direction satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_binding_direction_valid(const UmiUiReactiveBindingDirectionPolicy *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_binding_direction_archive_encode(const UmiUiReactiveBindingDirectionPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_binding_direction_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveBindingDirectionPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
