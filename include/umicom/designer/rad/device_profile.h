/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/device_profile.h
 *
 * PURPOSE:
 *   Describe preview device dimensions, density and input characteristics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_RAD_DEVICE_PROFILE_H
#define UMICOM_DESIGNER_RAD_DEVICE_PROFILE_H
#include "umicom/designer/rad/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadDeviceProfile {
    char profile_id[UMI_RAD_ID_CAPACITY];
    int32_t width;
    int32_t height;
    uint32_t dpi;
    bool touch;
} UmiRadDeviceProfile;
UmiStatus umi_rad_device_profile_init(UmiRadDeviceProfile *item);
int umi_rad_device_profile_is_valid(const UmiRadDeviceProfile *item);
#ifdef __cplusplus
}
#endif
#endif
