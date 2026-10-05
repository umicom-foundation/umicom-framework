/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/device_profile.h
 *
 * PURPOSE:
 *   Describe preview device dimensions, density and input characteristics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DEVICE_PROFILE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DEVICE_PROFILE_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer device profile data shared with callers of this public contract.
 */
typedef struct UmiRadDeviceProfile {
    char profile_id[UMI_RAD_ID_CAPACITY];
    int32_t width;
    int32_t height;
    uint32_t dpi;
    bool touch;
} UmiRadDeviceProfile;
/**
 * Initialise visual designer device profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_device_profile_init(UmiRadDeviceProfile *item);
/**
 * Check that visual designer device profile satisfies its contract before another service relies on
 * it.
 */
int umi_rad_device_profile_is_valid(const UmiRadDeviceProfile *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_device_profile_archive_encode(const UmiRadDeviceProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_device_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiRadDeviceProfile *value);

#ifdef __cplusplus
}
#endif
#endif
