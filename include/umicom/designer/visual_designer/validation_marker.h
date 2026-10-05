/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/validation_marker.h
 *
 * PURPOSE:
 *   Attach a validation severity/message to a component or property.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_VALIDATION_MARKER_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_VALIDATION_MARKER_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer validation marker data shared with callers of this public contract.
 */
typedef struct UmiRadValidationMarker {
    char component_id[UMI_RAD_ID_CAPACITY];
    char property_id[UMI_RAD_ID_CAPACITY];
    UmiRadSeverity severity;
    char message[UMI_RAD_TEXT_CAPACITY];
} UmiRadValidationMarker;
/**
 * Initialise visual designer validation marker from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_validation_marker_init(UmiRadValidationMarker *item);
/**
 * Check that visual designer validation marker satisfies its contract before another service relies on
 * it.
 */
int umi_rad_validation_marker_is_valid(const UmiRadValidationMarker *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_validation_marker_archive_encode(const UmiRadValidationMarker *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_validation_marker_archive_decode(const void *bytes, size_t byte_count,
    UmiRadValidationMarker *value);

#ifdef __cplusplus
}
#endif
#endif
