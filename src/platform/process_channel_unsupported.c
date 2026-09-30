/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_channel_unsupported.c
 * PURPOSE:
 *   Report unavailable process-channel operations without substituting a simulated process.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#include "process_channel_internal.h"
uint64_t PcMilliseconds(void){
    return 0;
}
UmiStatus UmiProcessChannelOpen(const UmiProcessChannelRequest *r,UmiProcessChannel **o){
    (void)r;
    if(o)*o=NULL;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus UmiProcessChannelRead(UmiProcessChannel*c,void*b,size_t n,size_t*o,unsigned t){
    (void)c;
    (void)b;
    (void)n;
    (void)t;
    if(o)*o=0;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus UmiProcessChannelWrite(UmiProcessChannel*c,const void*b,size_t n,unsigned t){
    (void)c;
    (void)b;
    (void)n;
    (void)t;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus UmiProcessChannelPoll(UmiProcessChannel*c,UmiProcessChannelSnapshot*o){
    (void)c;
    (void)o;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus UmiProcessChannelTerminate(UmiProcessChannel*c){
    (void)c;
    return UMI_STATUS_UNAVAILABLE;
}
void UmiProcessChannelDestroy(UmiProcessChannel*c){
    free(c);
}
