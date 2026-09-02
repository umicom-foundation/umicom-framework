/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/device_profile.h
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
#ifndef UMICOM_DESIGNER_RAD_DEVICE_PROFILE_H
#define UMICOM_DESIGNER_RAD_DEVICE_PROFILE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the rad device profile data shared with callers of this public contract.
 */
typedef struct UmiRadDeviceProfile {
    char profile_id[UMI_RAD_ID_CAPACITY];
    int32_t width;
    int32_t height;
    uint32_t dpi;
    bool touch;
} UmiRadDeviceProfile;
/**
 * Initialise rad device profile from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_device_profile_init(UmiRadDeviceProfile *item);
/**
 * Check that rad device profile satisfies its contract before another service relies on
 * it.
 */
int umi_rad_device_profile_is_valid(const UmiRadDeviceProfile *item);
#ifdef __cplusplus
}
#endif
#endif
