/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/event_binding.h
 *
 * PURPOSE:
 *   Bind a semantic component event to a Framework command identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_EVENT_BINDING_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_EVENT_BINDING_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer event binding data shared with callers of this public contract.
 */
typedef struct UmiRadEventBinding {
    char binding_id[UMI_RAD_ID_CAPACITY];
    char component_id[UMI_RAD_ID_CAPACITY];
    char event_id[UMI_RAD_ID_CAPACITY];
    char command_id[UMI_RAD_ID_CAPACITY];
    bool enabled;
} UmiRadEventBinding;
/**
 * Initialise visual designer event binding from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_event_binding_init(UmiRadEventBinding *item);
/**
 * Check that visual designer event binding satisfies its contract before another service relies on it.
 */
int umi_rad_event_binding_is_valid(const UmiRadEventBinding *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_event_binding_archive_encode(const UmiRadEventBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_event_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiRadEventBinding *value);

#ifdef __cplusplus
}
#endif
#endif
