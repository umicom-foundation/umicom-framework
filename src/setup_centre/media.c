/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/media.c
 * Prepare exact boot inputs and invoke an explicit GRUB provider. Do not turn
 * an arbitrary file copy into a claim of bootable media or write a raw drive.
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <inttypes.h>
static const char GRUB_CONFIG[]=
"set timeout=5\nset default=0\n"
"serial --unit=0 --speed=115200\nterminal_output console serial\n"
"menuentry 'Umicom OS normal (diskless foundation)' {\n"
"  linux /boot/kernel console=ttyS0 rdinit=/init panic=-1 umicom.recovery=0 umicom.autopoweroff=0\n"
"  initrd /boot/initrd\n}\n"
"menuentry 'Umicom OS recovery (diskless foundation)' {\n"
"  linux /boot/kernel console=ttyS0 rdinit=/init panic=-1 umicom.recovery=1 umicom.autopoweroff=0\n"
"  initrd /boot/initrd\n}\n";

UmiStatus UmiSetupMediaPrepare(const char *kernel,const char *initrd,const char *dest,UmiSetupReport *r)
{

    UmiSetupReport local= {
        0
    };
    if(!r)r=&local;
    memset(r,0,sizeof *r);

    if(UmiSetupValidateAbsolute(dest)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;

    UmiStatus s=ScKernel(kernel,"x86_64",r);
    unsigned char h[10];
    size_t n=0;
    if(s==UMI_STATUS_OK)s=ScHeaderRead(initrd,h,sizeof h,&n,r);

    if(s==UMI_STATUS_OK&&(n!=sizeof h||h[0]!=0x1fU||h[1]!=0x8bU||h[2]!=8U||(h[3]&0xe0U)))s=UMI_STATUS_PARSE_ERROR;

    char kh[65],ih[65],gh[65];
    uint64_t kb=0,ib=0;

    if(s==UMI_STATUS_OK)s=ScDigest(kernel,kh,&kb,NULL,NULL,r);
    if(s==UMI_STATUS_OK)s=ScDigest(initrd,ih,&ib,NULL,NULL,r);

    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(GRUB_CONFIG,sizeof GRUB_CONFIG-1U,gh);

    if(s==UMI_STATUS_OK)s=ScMakeDirectory(dest,r);
    if(s==UMI_STATUS_OK)r->outputCreated=1;

    if(s==UMI_STATUS_OK)s=ScParents(dest,"tree/boot/grub/grub.cfg",r);

    const char *sources[]= {
        kernel,initrd
    };
    const char *relative[]= {
        "tree/boot/kernel","tree/boot/initrd"
    };
    const char *hashes[]= {
        kh,ih
    };
    uint64_t sizes[]= {
        kb,ib
    };

    for(size_t i=0; i<2U&&s==UMI_STATUS_OK; ++i) {
        char target[UMI_SETUP_PATH_CAPACITY];
        s=ScJoin(dest,relative[i],target);
        if(s==UMI_STATUS_OK)s=ScCopy(sources[i],target,hashes[i],sizes[i],NULL,NULL,r);
    }

    if(s==UMI_STATUS_OK) {
        char target[UMI_SETUP_PATH_CAPACITY];
        s=ScJoin(dest,"tree/boot/grub/grub.cfg",target);
        if(s==UMI_STATUS_OK)s=ScWriteNew(target,GRUB_CONFIG,sizeof GRUB_CONFIG-1U,r);
    }

    if(s==UMI_STATUS_OK) {
        ScText text;
        ScTextInit(&text);
        ScPrint(&text,"UMICOM_MEDIA_SOURCE\t1\nx86_64\n%s\t%" PRIu64 "\n%s\t%" PRIu64 "\n%s\n",kh,kb,ih,ib,gh);
        char target[UMI_SETUP_PATH_CAPACITY];
        s=text.status;
        if(s==UMI_STATUS_OK)s=ScJoin(dest,"media.umi",target);
        if(s==UMI_STATUS_OK)s=ScWriteNew(target,text.data,text.size,r);
        ScTextFree(&text);
    }

    r->completed=s==UMI_STATUS_OK;
    ScReport(r,s,s==UMI_STATUS_OK?"Media source prepared, not a bootable image. GRUB must add the boot loader.":"Media preparation did not complete. No USB or optical drive was opened.");
    return s;

}

#ifdef __linux__
static UmiStatus VerifyPrepared(const char *root,char kernel[UMI_SETUP_PATH_CAPACITY],char initrd[UMI_SETUP_PATH_CAPACITY],char receiptHash[65],UmiSetupReport *r)
{

    char manifest[UMI_SETUP_PATH_CAPACITY],cfg[UMI_SETUP_PATH_CAPACITY];
    UmiStatus s=ScJoin(root,"media.umi",manifest);
    char *data=NULL;
    size_t length=0;

    if(s==UMI_STATUS_OK)s=ScRead(manifest,1024U,&data,&length,r);
    if(s!=UMI_STATUS_OK)return s;

    char *config=NULL;
    size_t configLength=0;
    s=ScJoin(root,"tree/boot/grub/grub.cfg",cfg);
    if(s==UMI_STATUS_OK)s=ScRead(cfg,4096U,&config,&configLength,r);

    if(s==UMI_STATUS_OK&&(configLength!=sizeof GRUB_CONFIG-1U||memcmp(config,GRUB_CONFIG,configLength)))s=UMI_STATUS_INVALID_STATE;
    free(config);

    if(s==UMI_STATUS_OK)s=ScJoin(root,"tree/boot/kernel",kernel);
    if(s==UMI_STATUS_OK)s=ScJoin(root,"tree/boot/initrd",initrd);

    char kh[65],ih[65],gh[65];
    uint64_t kb=0,ib=0;

    if(s==UMI_STATUS_OK)s=ScDigest(kernel,kh,&kb,NULL,NULL,r);
    if(s==UMI_STATUS_OK)s=ScDigest(initrd,ih,&ib,NULL,NULL,r);

    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(GRUB_CONFIG,sizeof GRUB_CONFIG-1U,gh);

    ScText expected;
    ScTextInit(&expected);
    if(s==UMI_STATUS_OK) {
        ScPrint(&expected,"UMICOM_MEDIA_SOURCE\t1\nx86_64\n%s\t%" PRIu64 "\n%s\t%" PRIu64 "\n%s\n",kh,kb,ih,ib,gh);
        s=expected.status;
        if(s==UMI_STATUS_OK&&(expected.size!=length||memcmp(expected.data,data,length)))s=UMI_STATUS_INVALID_STATE;
    }

    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(data,length,receiptHash);
    ScTextFree(&expected);
    free(data);
    return s;

}

static uint32_t Le32(const unsigned char *p) {
    return (uint32_t)p[0]|((uint32_t)p[1]<<8U)|((uint32_t)p[2]<<16U)|((uint32_t)p[3]<<24U);
}

static UmiStatus IsoCheck(const char *path,UmiSetupReport *r)
{

    unsigned char sector[2048];
    uint64_t size=0;
    int primary=0;
    uint32_t catalogue=0;

    for(uint64_t i=16; i<48; ++i) {
        UmiStatus s=ScReadAt(path,i*2048U,sector,sizeof sector,&size,r);
        if(s!=UMI_STATUS_OK)return s;

        if(memcmp(sector+1,"CD001",5)||sector[6]!=1U)return UMI_STATUS_PARSE_ERROR;

        if(sector[0]==1U) {
            primary=1;
            if((uint64_t)Le32(sector+80)*2048U>size||sector[128]!=0U||sector[129]!=8U)return UMI_STATUS_PARSE_ERROR;
        }

        if(sector[0]==0U&&!memcmp(sector+7,"EL TORITO SPECIFICATION",23))catalogue=Le32(sector+71);

        if(sector[0]==255U)break;

    }

    if(!primary||!catalogue)return UMI_STATUS_PARSE_ERROR;

    UmiStatus s=ScReadAt(path,(uint64_t)catalogue*2048U,sector,64,&size,r);
    if(s!=UMI_STATUS_OK)return s;

    unsigned sum=0;
    for(unsigned i=0; i<32; i+=2)sum+=(unsigned)sector[i]|((unsigned)sector[i+1U]<<8U);

    if((sum&65535U)||sector[0]!=1U||sector[30]!=0x55U||sector[31]!=0xaaU||sector[32]!=0x88U)return UMI_STATUS_PARSE_ERROR;

    return UMI_STATUS_OK;

}

#endif
UmiStatus UmiSetupMediaBuildIso(const char *root,const char *grubTool,const char *dest,UmiSetupExecute execute,void *context,UmiSetupReport *r)
{

    if(r)memset(r,0,sizeof *r);
#ifndef __linux__
    (void)root;
    (void)grubTool;
    (void)dest;
    (void)execute;
    (void)context;

    ScReport(r,UMI_STATUS_UNAVAILABLE,"ISO production requires the native Linux GRUB tool. Prepare sources on Windows, then use the Linux build.");
    return UMI_STATUS_UNAVAILABLE;

#else
    if(!execute||UmiSetupValidateAbsolute(grubTool)!=UMI_STATUS_OK||UmiSetupValidateAbsolute(dest)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;

    char kernel[UMI_SETUP_PATH_CAPACITY],initrd[UMI_SETUP_PATH_CAPACITY];
    char reviewedReceipt[65];
    UmiStatus s=VerifyPrepared(root,kernel,initrd,reviewedReceipt,r);
    if(s!=UMI_STATUS_OK)return s;

    /* A selected tool must be a native host program, not a script that the
     * general-purpose process runner might otherwise give to an interpreter. */
    s=ScHostProgram(grubTool,r);
    if(s!=UMI_STATUS_OK)return s;

    /* Copy only the verified three files into a NEW tree. Unlisted files added
     * to the review tree are never given to the external ISO producer. */

    s=UmiSetupMediaPrepare(kernel,initrd,dest,r);
    if(s!=UMI_STATUS_OK)return s;

    /* Bind the copied inputs to the receipt already reviewed above. A source
     * change between inspection and copying must not silently become a new
     * accepted media build merely because a new receipt could be generated. */
    char copiedReceipt[UMI_SETUP_PATH_CAPACITY],copiedHash[65];
    uint64_t copiedBytes=0;
    s=ScJoin(dest,"media.umi",copiedReceipt);
    if(s==UMI_STATUS_OK)s=ScDigest(copiedReceipt,copiedHash,&copiedBytes,NULL,NULL,r);
    if(s==UMI_STATUS_OK&&strcmp(copiedHash,reviewedReceipt))s=UMI_STATUS_INVALID_STATE;
    if(s!=UMI_STATUS_OK) {
        if(r)r->completed=0;
        ScReport(r,s,"Media inputs changed after review. The partial new tree is retained; no producer was started.");
        return s;
    }

    char iso[UMI_SETUP_PATH_CAPACITY],tree[UMI_SETUP_PATH_CAPACITY];
    s=ScJoin(dest,"UmicomOS.iso",iso);
    if(s==UMI_STATUS_OK)s=ScJoin(dest,"tree",tree);

    int code=-1;
    const char *arguments[]= {
        "-o",iso,tree
    };
    if(r)r->completed=0;

    if(s==UMI_STATUS_OK)s=execute(grubTool,arguments,3,dest,context,&code);
    if(s==UMI_STATUS_OK&&code!=0)s=UMI_STATUS_IO_ERROR;

    if(s==UMI_STATUS_OK)s=IsoCheck(iso,r);

    if(r)r->completed=s==UMI_STATUS_OK;
    ScReport(r,s,s==UMI_STATUS_OK?"ISO9660 and El Torito structures checked. Booting, USB writing and optical burning have not been performed.":"ISO production was not completed. Partial new files are retained; no physical drive was opened.");
    return s;

#endif
}
