/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/io_unsupported.c
 * PURPOSE:
 *   Explicit unsupported-host adapter; pure buffer validation remains available.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Explicit unsupported-host adapter; pure buffer validation remains available. */
#include "internal.h"
UmiStatus OiRead(const char *p,size_t l,unsigned char **d,size_t *n) {
    (void)p;
    (void)l;
    if(d)*d=NULL;
    if(n)*n=0;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus OiWrite(const char *p,const void *d,size_t n) {
    (void)p;
    (void)d;
    (void)n;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus OiDirectory(const char *p,int c) {
    (void)p;
    (void)c;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus OiParents(const char *p,const char *n) {
    (void)p;
    (void)n;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus OiInventory(const char *p,const char *const *n,size_t c) {
    (void)p;
    (void)n;
    (void)c;
    return UMI_STATUS_UNAVAILABLE;
}
int OiNormalLinuxUser(void) {
    return 0;
}
UmiStatus OiNativeProgram(const char *p) {
    (void)p;
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus OiLock(const char *r,void **l) {
    (void)r;
    if(l)*l=NULL;
    return UMI_STATUS_UNAVAILABLE;
}
void OiUnlock(void *l) {
    (void)l;
}
