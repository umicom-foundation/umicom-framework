/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/component_catalogue_bridge.h
 *
 * PURPOSE:
 *   Map Design System component identifiers to canonical designer component types.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_COMPONENT_CATALOGUE_BRIDGE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_COMPONENT_CATALOGUE_BRIDGE_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer component catalogue bridge data shared with callers of this public
 * contract.
 */
typedef struct UmiRadComponentCatalogueBridge {
    char design_component_id[UMI_RAD_ID_CAPACITY];
    char designer_type[UMI_RAD_ID_CAPACITY];
    char family[UMI_RAD_ID_CAPACITY];
    bool available;
} UmiRadComponentCatalogueBridge;
/**
 * Initialise visual designer component catalogue bridge from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_rad_component_catalogue_bridge_init(UmiRadComponentCatalogueBridge *item);
/**
 * Check that visual designer component catalogue bridge satisfies its contract before another service
 * relies on it.
 */
int umi_rad_component_catalogue_bridge_is_valid(const UmiRadComponentCatalogueBridge *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_component_catalogue_bridge_archive_encode(const UmiRadComponentCatalogueBridge *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_component_catalogue_bridge_archive_decode(const void *bytes, size_t byte_count,
    UmiRadComponentCatalogueBridge *value);

#ifdef __cplusplus
}
#endif
#endif
