/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/device_profile.c
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
#include "umicom/designer/rad/device_profile.h"
#include <string.h>
UmiStatus umi_rad_device_profile_init(UmiRadDeviceProfile *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->profile_id, sizeof item->profile_id, "device_profile");
    item->width = 100;
    item->height = 100;
    item->dpi = 96U;
    return UMI_STATUS_OK;
}
int umi_rad_device_profile_is_valid(const UmiRadDeviceProfile *item){if(item==NULL)return 0;return item->width > 0 && item->height > 0 && item->dpi > 0U;}
