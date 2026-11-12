/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/boot_media/device_unsupported.c
 *
 * PURPOSE:
 *   Report unavailable native device operations on unsupported host platforms.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
/* The former public adapter entry remains below for source review. Shared
 * discovery.c now resets and publishes diagnostics consistently on every host;
 * this private entry retains the same native observation and refusal logic. */
#if 0
UmiStatus UmiBootMediaDiscover(UmiBootMediaInventory *out,UmiBootMediaReport *r)
#else
UmiStatus BmDiscover(UmiBootMediaInventory *out,UmiBootMediaReport *r)
#endif
{
    if (out)memset(out,0,sizeof *out);
    return BmMessage(r,UMI_STATUS_UNAVAILABLE,"No native removable-device adapter for this host.");
}

UmiStatus BmDeviceCheck(const char*p,UmiBootMediaDevice*d) {
    (void)p;
    (void)d;
    return UMI_STATUS_UNAVAILABLE;
}

UmiStatus BmDeviceOpen(const UmiBootMediaDevice*d,int w,BmFile**out) {
    (void)d;
    (void)w;
    if (out)*out=NULL;
    return UMI_STATUS_UNAVAILABLE;
}
