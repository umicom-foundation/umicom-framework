/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/component_instance.h
 *
 * PURPOSE:
 *   Represent one semantic component instance on a designer document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_COMPONENT_INSTANCE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_COMPONENT_INSTANCE_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer component instance data shared with callers of this public contract.
 */
typedef struct UmiRadComponentInstance {
    char component_id[UMI_RAD_ID_CAPACITY];
    char component_type[UMI_RAD_ID_CAPACITY];
    char parent_id[UMI_RAD_ID_CAPACITY];
    UmiRadRect bounds;
    bool visible;
} UmiRadComponentInstance;
/**
 * Initialise visual designer component instance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_component_instance_init(UmiRadComponentInstance *item);
/**
 * Check that visual designer component instance satisfies its contract before another service relies
 * on it.
 */
int umi_rad_component_instance_is_valid(const UmiRadComponentInstance *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_component_instance_archive_encode(const UmiRadComponentInstance *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_component_instance_archive_decode(const void *bytes, size_t byte_count,
    UmiRadComponentInstance *value);

#ifdef __cplusplus
}
#endif
#endif
