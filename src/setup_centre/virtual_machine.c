/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/virtual_machine.c
 * Explicit, provider-neutral execution boundary with a QEMU plan adapter.
 * Input inspection never launches a program. A guest gets no host storage or
 * network device from this profile; that is not a guarantee against VM bugs.
 *---------------------------------------------------------------------------*/

#include "internal.h"
static uint16_t Little16(const unsigned char *p) {
    return (uint16_t)((uint16_t)p[0]|((uint16_t)p[1]<<8U));
}

static uint32_t Little32(const unsigned char *p) {
    return (uint32_t)p[0]|((uint32_t)p[1]<<8U)|((uint32_t)p[2]<<16U)|((uint32_t)p[3]<<24U);
}

UmiStatus ScKernel(const char *path,const char *arch,UmiSetupReport *r)
{

    unsigned char header[1024];
    size_t n=0;
    UmiStatus s=ScHeaderRead(path,header,sizeof header,&n,r);
    if(s!=UMI_STATUS_OK)return s;

    if(!strcmp(arch,"x86_64")) {
        if(n<0x238U||header[0x1fe]!=0x55U||header[0x1ff]!=0xaaU||memcmp(header+0x202,"HdrS",4)||!(Little16(header+0x236)&1U))s=UMI_STATUS_PARSE_ERROR;
    }

    else if(!strcmp(arch,"riscv64")) {
        if(n<64U||memcmp(header+48,"RISCV\0\0\0",8)||memcmp(header+56,"RSC\x05",4))s=UMI_STATUS_PARSE_ERROR;
    }

    else s=UMI_STATUS_INVALID_ARGUMENT;

    if(s!=UMI_STATUS_OK)ScReport(r,s,"The kernel header does not match the selected guest architecture.");
    return s;

}

UmiStatus ScHostProgram(const char *path,UmiSetupReport *r)
{

    unsigned char h[4096];
    size_t n=0;
    UmiStatus s=ScHeaderRead(path,h,sizeof h,&n,r);
    if(s!=UMI_STATUS_OK)return s;

#ifdef _WIN32
    if(n<64U||h[0]!='M'||h[1]!='Z')return UMI_STATUS_PARSE_ERROR;

    uint32_t offset=Little32(h+0x3c);
    if(offset>n||n-offset<26U||memcmp(h+offset,"PE\0\0",4))return UMI_STATUS_PARSE_ERROR;

    if(Little16(h+offset+4)!=0x8664U||(Little16(h+offset+22)&0x2000U))return UMI_STATUS_PARSE_ERROR;

#else
    (void)Little32;

    if(n<64U||memcmp(h,"\x7f" "ELF\x02\x01\x01",7)||(Little16(h+16)!=2U&&Little16(h+16)!=3U))return UMI_STATUS_PARSE_ERROR;

#if defined(__x86_64__)
    if(Little16(h+18)!=62U)return UMI_STATUS_PARSE_ERROR;

#elif defined(__aarch64__)
    if(Little16(h+18)!=183U)return UMI_STATUS_PARSE_ERROR;

#elif defined(__riscv) && __riscv_xlen == 64
    if(Little16(h+18)!=243U)return UMI_STATUS_PARSE_ERROR;

#else
    return UMI_STATUS_UNAVAILABLE;

#endif
#endif
    return UMI_STATUS_OK;

}

static UmiStatus Arg(UmiSetupVmPlan *p,const char *s)
{
    if(p->argumentCount==UMI_SETUP_VM_MAX_ARGUMENTS||strlen(s)>=UMI_SETUP_VM_ARGUMENT_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(p->arguments[p->argumentCount++],s);
    return UMI_STATUS_OK;
}

UmiStatus UmiSetupVmReview(const UmiSetupVmConfig *c,UmiSetupVmPlan *p,UmiSetupReport *r)
{

    if(r)memset(r,0,sizeof *r);

    if(!c||!p||UmiSetupValidateAbsolute(c->qemu)!=UMI_STATUS_OK||UmiSetupValidateAbsolute(c->kernel)!=UMI_STATUS_OK||UmiSetupValidateAbsolute(c->initrd)!=UMI_STATUS_OK||!c->architecture||
    (strcmp(c->architecture,"x86_64")&&strcmp(c->architecture,"riscv64"))||c->memoryMiB<256U||c->memoryMiB>8192U||!c->processors||c->processors>8U||
    (c->interactive!=0&&c->interactive!=1)||(c->recovery!=0&&c->recovery!=1)||strchr(c->initrd,','))return UMI_STATUS_INVALID_ARGUMENT;

    memset(p,0,sizeof *p);
    strcpy(p->program,c->qemu);
    strcpy(p->kernel,c->kernel);
    strcpy(p->initrd,c->initrd);
    strcpy(p->architecture,c->architecture);
    p->config=*c;

    p->config.qemu=p->program;
    p->config.kernel=p->kernel;
    p->config.initrd=p->initrd;
    p->config.architecture=p->architecture;

    UmiStatus s=ScHostProgram(c->qemu,r);
    if(s==UMI_STATUS_OK)s=ScKernel(c->kernel,c->architecture,r);

    unsigned char gzip[16];
    size_t n=0;
    if(s==UMI_STATUS_OK)s=ScHeaderRead(c->initrd,gzip,sizeof gzip,&n,r);

    if(s==UMI_STATUS_OK&&(n<10U||gzip[0]!=0x1fU||gzip[1]!=0x8bU||gzip[2]!=8U||(gzip[3]&0xe0U)))s=UMI_STATUS_PARSE_ERROR;

    uint64_t bytes;
    if(s==UMI_STATUS_OK)s=ScDigest(c->qemu,p->programHash,&bytes,NULL,NULL,r);

    if(s==UMI_STATUS_OK)s=ScDigest(c->kernel,p->kernelHash,&bytes,NULL,NULL,r);

    if(s==UMI_STATUS_OK)s=ScDigest(c->initrd,p->initrdHash,&bytes,NULL,NULL,r);

    if(s!=UMI_STATUS_OK) {
        ScReport(r,s,"VM inputs could not be reviewed. Use a trusted native QEMU program and matching kernel/initramfs.");
        return s;
    }

    const char *common[]= {
        "-no-user-config","-nodefaults","-name","Umicom OS","-M",!strcmp(c->architecture,"riscv64")?"virt":"q35","-accel","tcg","-m"
    };

    for(size_t i=0; i<sizeof common/sizeof common[0]; ++i)(void)Arg(p,common[i]);

    char number[24];
    (void)snprintf(number,sizeof number,"%uM",c->memoryMiB);
    (void)Arg(p,number);
    (void)Arg(p,"-smp");
    (void)snprintf(number,sizeof number,"%u",c->processors);
    (void)Arg(p,number);

    if(!strcmp(c->architecture,"riscv64")) {
        (void)Arg(p,"-bios");
        (void)Arg(p,"default");
    }

    if(!c->interactive) {
        (void)Arg(p,"-display");
        (void)Arg(p,"none");
    }

    (void)Arg(p,"-serial");
    (void)Arg(p,c->interactive?"vc":"stdio");
    (void)Arg(p,"-monitor");
    (void)Arg(p,"none");
    (void)Arg(p,"-nic");
    (void)Arg(p,"none");
    (void)Arg(p,"-no-reboot");

    (void)Arg(p,"-kernel");
    (void)Arg(p,c->kernel);
    (void)Arg(p,"-initrd");
    (void)Arg(p,c->initrd);
    (void)Arg(p,"-append");

    char command[192];
    (void)snprintf(command,sizeof command,"console=ttyS0 rdinit=/init panic=-1 umicom.recovery=%d umicom.autopoweroff=%d",c->recovery,c->interactive?0:1);
    (void)Arg(p,command);

    ScText t;
    ScTextInit(&t);
    ScPrint(&t,"Umicom QEMU plan\n%s\n%s\n%s\n%zu:%s\n",p->programHash,p->kernelHash,p->initrdHash,strlen(p->program),p->program);

    for(size_t i=0; i<p->argumentCount; ++i)ScPrint(&t,"%zu:%s\n",strlen(p->arguments[i]),p->arguments[i]);

    s=t.status;
    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(t.data,t.size,p->fingerprint);
    ScTextFree(&t);

    ScReport(r,s,"VM plan reviewed. Header and hash checks are not boot acceptance or publisher verification.");
    return s;

}

UmiStatus UmiSetupVmRun(const UmiSetupVmPlan *p,const char *expected,UmiSetupExecute execute,void *context,UmiSetupReport *r)
{

    if(r)memset(r,0,sizeof *r);
    if(!p||!execute||!ScHashValid(expected)||strcmp(p->fingerprint,expected))return UMI_STATUS_INVALID_ARGUMENT;

    UmiSetupVmPlan *fresh=malloc(sizeof *fresh);
    if(!fresh)return UMI_STATUS_OUT_OF_MEMORY;

    UmiStatus s=UmiSetupVmReview(&p->config,fresh,r);

    if(s==UMI_STATUS_OK&&strcmp(fresh->fingerprint,expected))s=UMI_STATUS_INVALID_STATE;

    if(s==UMI_STATUS_OK) {
        const char *args[UMI_SETUP_VM_MAX_ARGUMENTS];
        for(size_t i=0; i<fresh->argumentCount; ++i)args[i]=fresh->arguments[i];

        char directory[UMI_SETUP_PATH_CAPACITY];
        strcpy(directory,fresh->program);
        char *last=strrchr(directory,'/'),*back=strrchr(directory,'\\');
        if(back&&(!last||back>last))last=back;
        if(last==directory)last[1]=0;
        else if(last==directory+2&&directory[1]==':')last[1]=0;
        else if(last)*last=0;

        int code=-1;
        s=execute(fresh->program,args,fresh->argumentCount,directory,context,&code);
        if(s==UMI_STATUS_OK&&code!=0)s=UMI_STATUS_IO_ERROR;

        if(r)r->completed=s==UMI_STATUS_OK;
        ScReport(r,s,s==UMI_STATUS_OK?"QEMU exited successfully. Guest boot acceptance must be checked separately.":"QEMU did not complete successfully; inspect the captured diagnostics.");
    }

    else ScReport(r,s,"VM inputs or review fingerprint changed. Nothing was launched.");
    free(fresh);
    return s;

}
