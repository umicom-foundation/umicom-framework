/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/form_descriptor.h
 *
 * PURPOSE:
 *   Describe a form, semantic root and Framework submit command.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_FORM_DESCRIPTOR_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_FORM_DESCRIPTOR_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer form descriptor data shared with callers of this public contract.
 */
typedef struct UmiRadFormDescriptor {
    char form_id[UMI_RAD_ID_CAPACITY];
    char title[UMI_RAD_TEXT_CAPACITY];
    char root_component_id[UMI_RAD_ID_CAPACITY];
    char submit_command_id[UMI_RAD_ID_CAPACITY];
} UmiRadFormDescriptor;
/**
 * Initialise visual designer form descriptor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_form_descriptor_init(UmiRadFormDescriptor *item);
/**
 * Check that visual designer form descriptor satisfies its contract before another service relies on
 * it.
 */
int umi_rad_form_descriptor_is_valid(const UmiRadFormDescriptor *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_form_descriptor_archive_encode(const UmiRadFormDescriptor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_form_descriptor_archive_decode(const void *bytes, size_t byte_count,
    UmiRadFormDescriptor *value);

#ifdef __cplusplus
}
#endif
#endif
