/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/boot_media/device_unsupported.c
 * PURPOSE:
 *   Report unavailable native device operations on unsupported host platforms.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */

#include "internal.h"
UmiStatus UmiBootMediaDiscover(UmiBootMediaInventory *out,UmiBootMediaReport *r) {
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
