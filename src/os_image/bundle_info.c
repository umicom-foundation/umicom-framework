/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/bundle_info.c
 * PURPOSE:
 *   Consumers receive validated paths and identity, never private bundle fields. The earlier
 *   verify/boot entry points and their implementations are unchanged.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Consumers receive validated paths and identity, never private bundle fields.
 * The earlier verify/boot entry points and their implementations are unchanged. */
#include "internal.h"
#include "umicom/os_image/bundle_info.h"
UmiStatus UmiOsImageDescribe(const char *root,UmiOsImageBundleInfo *out,UmiOsImageReport *report){
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof *out);
    UmiOsImageReport local;
    if(!report)report=&local;
    char manifestPath[UMI_OS_IMAGE_PATH],before[65],after[65];
    uint64_t beforeSize=0,afterSize=0;
    UmiStatus s=OiJoin(root,"image.umi",manifestPath);
    if(s==UMI_STATUS_OK)s=OiDigest(manifestPath,before,&beforeSize);
    if(s==UMI_STATUS_OK)s=UmiOsImageVerify(root,report);
    OiBundle b;
    if(s==UMI_STATUS_OK)s=OiBundleLoad(root,&b);
    if(s==UMI_STATUS_OK&&strcmp(b.sourceId,report->sourceId))s=UMI_STATUS_INVALID_STATE;
    if(s==UMI_STATUS_OK)s=OiDigest(manifestPath,after,&afterSize);
    if(s==UMI_STATUS_OK&&(beforeSize!=afterSize||strcmp(before,after)))s=UMI_STATUS_INVALID_STATE;
    if(s==UMI_STATUS_OK){
        out->architecture=b.arch;
        strcpy(out->sourceId,b.sourceId);
        s=OiJoin(root,OiKernelName(b.arch),out->kernel);
    }
    if(s==UMI_STATUS_OK)s=OiJoin(root,"umicom-rootfs.cpio.gz",out->rootfs);
    if(s==UMI_STATUS_OK){
        char p[UMI_OS_IMAGE_PATH];
        uint64_t n;
        s=OiJoin(root,"image.umi",p);
        if(s==UMI_STATUS_OK)s=OiDigest(p,out->manifestHash,&n);
    }
    if(s!=UMI_STATUS_OK)memset(out,0,sizeof *out);
    return s;
}
