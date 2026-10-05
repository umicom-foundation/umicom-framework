/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/property_binding.h
 *
 * PURPOSE:
 *   Describe a visual property binding backed by the canonical reactive UI state layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_PROPERTY_BINDING_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_PROPERTY_BINDING_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer property binding data shared with callers of this public contract.
 */
typedef struct UmiRadPropertyBinding {
    char binding_id[UMI_RAD_ID_CAPACITY];
    char source_path[UMI_RAD_PATH_CAPACITY];
    char target_path[UMI_RAD_PATH_CAPACITY];
    bool two_way;
    bool enabled;
} UmiRadPropertyBinding;
/**
 * Initialise visual designer property binding from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_property_binding_init(UmiRadPropertyBinding *item);
/**
 * Check that visual designer property binding satisfies its contract before another service relies on
 * it.
 */
int umi_rad_property_binding_is_valid(const UmiRadPropertyBinding *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_property_binding_archive_encode(const UmiRadPropertyBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_property_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiRadPropertyBinding *value);

#ifdef __cplusplus
}
#endif
#endif
