/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/event_descriptor.h
 *
 * PURPOSE:
 *   Describe an event exposed by a semantic component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_EVENT_DESCRIPTOR_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_EVENT_DESCRIPTOR_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer event descriptor data shared with callers of this public contract.
 */
typedef struct UmiRadEventDescriptor {
    char event_id[UMI_RAD_ID_CAPACITY];
    char label[UMI_RAD_TEXT_CAPACITY];
    char parameter_type[UMI_RAD_ID_CAPACITY];
    bool bindable;
} UmiRadEventDescriptor;
/**
 * Initialise visual designer event descriptor from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_event_descriptor_init(UmiRadEventDescriptor *item);
/**
 * Check that visual designer event descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_event_descriptor_is_valid(const UmiRadEventDescriptor *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_event_descriptor_archive_encode(const UmiRadEventDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_event_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiRadEventDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
