/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/responsive_variant.h
 *
 * PURPOSE:
 *   Describe per-breakpoint component geometry and visibility overrides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_RESPONSIVE_VARIANT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_RESPONSIVE_VARIANT_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer responsive variant data shared with callers of this public contract.
 */
typedef struct UmiRadResponsiveVariant {
    char breakpoint_id[UMI_RAD_ID_CAPACITY];
    UmiRadRect bounds;
    bool visible;
    bool override_geometry;
} UmiRadResponsiveVariant;
/**
 * Initialise visual designer responsive variant from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_responsive_variant_init(UmiRadResponsiveVariant *item);
/**
 * Check that visual designer responsive variant satisfies its contract before another service relies
 * on it.
 */
int umi_rad_responsive_variant_is_valid(const UmiRadResponsiveVariant *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_responsive_variant_archive_encode(const UmiRadResponsiveVariant *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_responsive_variant_archive_decode(const void *bytes, size_t byte_count,
    UmiRadResponsiveVariant *value);

#ifdef __cplusplus
}
#endif
#endif
