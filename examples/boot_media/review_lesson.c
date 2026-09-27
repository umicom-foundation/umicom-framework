/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A media review needs both image facts and device facts. This lesson evaluates
 * fictional observations only; the actual service must re-observe the device.
 *---------------------------------------------------------------------------*/

#include "umicom/boot_media/media.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    UmiBootMediaImage image= {
        0
    };
    image.bytes=64U*1024U*1024U;
    image.mbrLayout=1;
    UmiBootMediaDevice device= {
        0
    };
    strcpy(device.path,"practice-device-not-a-path");
    strcpy(device.identity,"classroom-observation");
    strcpy(device.serial,"PRACTICE-ONLY");
    strcpy(device.model,"Umicom training disk");
    device.bytes=UINT64_C(8)*1024U*1024U*1024U;
    device.sectorBytes=512U;
    device.usb=1;
    device.removable=1;
    UmiBootMediaReport report= {
        0
    };
    if (UmiBootMediaCheckDevicePolicy(&image,&device,&report)!=UMI_STATUS_OK)return 1;
    puts("The fictional image fits the fictional USB disk.");
    device.protectedDevice=1;
    if (UmiBootMediaCheckDevicePolicy(&image,&device,&report)!=UMI_STATUS_PERMISSION_DENIED)return 1;
    puts("Marking the disk protected refuses the operation.");
    puts("Practice complete. No file, physical disk, optical drive or guest was opened.");
    return 0;
}
