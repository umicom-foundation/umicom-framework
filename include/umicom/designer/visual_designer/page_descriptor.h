/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/page_descriptor.h
 *
 * PURPOSE:
 *   Describe a visual application page, route and semantic root component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PAGE_DESCRIPTOR_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PAGE_DESCRIPTOR_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer page descriptor data shared with callers of this public contract.
 */
typedef struct UmiRadPageDescriptor {
    char page_id[UMI_RAD_ID_CAPACITY];
    char route[UMI_RAD_PATH_CAPACITY];
    char title[UMI_RAD_TEXT_CAPACITY];
    char root_component_id[UMI_RAD_ID_CAPACITY];
} UmiRadPageDescriptor;
/**
 * Initialise visual designer page descriptor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_page_descriptor_init(UmiRadPageDescriptor *item);
/**
 * Check that visual designer page descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_page_descriptor_is_valid(const UmiRadPageDescriptor *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_page_descriptor_archive_encode(const UmiRadPageDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_page_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiRadPageDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
