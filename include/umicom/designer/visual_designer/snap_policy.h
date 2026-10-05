/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/snap_policy.h
 *
 * PURPOSE:
 *   Configure grid, guide and component snapping tolerance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_SNAP_POLICY_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_SNAP_POLICY_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer snap policy data shared with callers of this public contract.
 */
typedef struct UmiRadSnapPolicy {
    bool grid_enabled;
    bool guides_enabled;
    bool components_enabled;
    int32_t tolerance;
} UmiRadSnapPolicy;
/**
 * Initialise visual designer snap policy from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_snap_policy_init(UmiRadSnapPolicy *item);
/**
 * Check that visual designer snap policy satisfies its contract before another service relies on it.
 */
int umi_rad_snap_policy_is_valid(const UmiRadSnapPolicy *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_snap_policy_archive_encode(const UmiRadSnapPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_snap_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiRadSnapPolicy *value);

#ifdef __cplusplus
}
#endif
#endif
