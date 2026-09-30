/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/build.c
 * PURPOSE:
 *   Explicit Buildroot orchestration. Source recipes are selected by the OS; this module
 *   owns native process execution, state boundaries and evidence.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Explicit Buildroot orchestration. Source recipes are selected by the OS;
 * this module owns native process execution, state boundaries and evidence. */
#include "internal.h"
#include "umicom/native_launcher/stage.h"
static int Setting(const char *text,const char *key,const char *expected) {
    size_t keySize=strlen(key),matches=0;
    int correct=0;
    for(const char *at=text;*at;) {
        const char *end=strchr(at,'\n');
        if(!end)return 0;
        size_t n=(size_t)(end-at);
        if(n>keySize&&!strncmp(at,key,keySize)&&at[keySize]=='=') {
            ++matches;
            correct=strlen(expected)==n-keySize-1U&&!memcmp(at+keySize+1U,expected,n-keySize-1U);
        }
        else if(n==keySize+13U&&!strncmp(at,"# ",2)&&!strncmp(at+2,key,keySize)&&!memcmp(at+2+keySize," is not set",11)) {
            ++matches;
            correct=!strcmp(expected,"n");
        }
        at=end+1;
    }
    return matches==1U&&correct;
}
UmiStatus OiConfigCheck(const char *root,const OiPlan *p,int kernel) {
    char relative[UMI_OS_IMAGE_NAME];
    if(kernel) {
        int n=snprintf(relative,sizeof relative,"build/build/linux-%s/.config",p->linuxVersion);
        if(n<0||(size_t)n>=sizeof relative)return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    else strcpy(relative,"build/.config");
    unsigned char *data=NULL;
    size_t size=0;
    UmiStatus s=OiReadJoined(root,relative,2U*1024U*1024U,&data,&size);
    if(s!=UMI_STATUS_OK)return s;
    if(memchr(data,0,size)||!size||data[size-1U]!='\n') {
        free(data);
        return UMI_STATUS_PARSE_ERROR;
    }
    const char *text=(const char*)data;
    if(kernel) {
        const char *yes[]= {
            "CONFIG_BLK_DEV_INITRD","CONFIG_RD_GZIP","CONFIG_BINFMT_ELF","CONFIG_PROC_FS","CONFIG_SYSFS","CONFIG_TMPFS","CONFIG_DEVTMPFS","CONFIG_SERIAL_8250_CONSOLE"
        }
        ;
        for(size_t i=0;i<sizeof yes/sizeof yes[0];++i)if(!Setting(text,yes[i],"y"))s=UMI_STATUS_INVALID_STATE;
        if(!Setting(text,"CONFIG_MODULES","n")||!Setting(text,"CONFIG_NET","n"))s=UMI_STATUS_INVALID_STATE;
        if(!Setting(text,p->arch==UMI_OS_IMAGE_X86_64?"CONFIG_X86_64":"CONFIG_RISCV","y"))s=UMI_STATUS_INVALID_STATE;
    }
    else {
        const char *yes[]= {
            "BR2_INIT_NONE","BR2_STATIC_LIBS","BR2_TOOLCHAIN_BUILDROOT_MUSL","BR2_PACKAGE_UMICOM_OS_INIT","BR2_PACKAGE_UMICOM_FRAMEWORK_PROBE","BR2_DOWNLOAD_FORCE_CHECK_HASHES","BR2_REPRODUCIBLE"
        }
        ;
        for(size_t i=0;i<sizeof yes/sizeof yes[0];++i)if(!Setting(text,yes[i],"y"))s=UMI_STATUS_INVALID_STATE;
        char version[40];
        int n=snprintf(version,sizeof version,"\"%s\"",p->linuxVersion);
        if(n<0||(size_t)n>=sizeof version||!Setting(text,"BR2_LINUX_KERNEL_CUSTOM_VERSION_VALUE",version)|| !Setting(text,p->arch==UMI_OS_IMAGE_X86_64?"BR2_x86_64":"BR2_riscv","y"))s=UMI_STATUS_INVALID_STATE;
    }
    free(data);
    return s;
}
static UmiStatus ConfigStamp(const char *root,const OiPlan *p,OiText *text) {
    char path[UMI_OS_IMAGE_PATH],hash[65];
    uint64_t size=0;
    UmiStatus s=OiConfigCheck(root,p,0);
    if(s==UMI_STATUS_OK)s=OiJoin(root,"build/.config",path);
    if(s==UMI_STATUS_OK)s=OiDigest(path,hash,&size);
    if(s==UMI_STATUS_OK) {
        OiPrint(text,"UMICOM_OS_CONFIGURED\t1\nsource\t%s\nconfig\t%s\t%" PRIu64 "\n",p->sourceId,hash,size);
        s=text->status;
    }
    return s;
}
static UmiStatus Matches(const char *root,const char *name,const OiText *t) {
    unsigned char *bytes=NULL;
    size_t n=0;
    UmiStatus s=OiReadJoined(root,name,OI_META_LIMIT,&bytes,&n);
    if(s==UMI_STATUS_OK&&(n!=t->size||memcmp(bytes,t->data,n)))s=UMI_STATUS_INVALID_STATE;
    free(bytes);
    return s;
}
static UmiStatus BuildStamp(const char *root,const OiPlan *p,OiText *t) {
    UmiStatus s=OiConfigCheck(root,p,1);
    if(s!=UMI_STATUS_OK)return s;
    char kernel[UMI_OS_IMAGE_NAME],config[UMI_OS_IMAGE_NAME];
    int a=snprintf(kernel,sizeof kernel,"build/images/%s",OiKernelName(p->arch));
    int b=snprintf(config,sizeof config,"build/build/linux-%s/.config",p->linuxVersion);
    if(a<0||b<0||(size_t)a>=sizeof kernel||(size_t)b>=sizeof config)return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *files[]= {
        "build/target/init","build/target/usr/libexec/umicom-platform-check","build/target/usr/libexec/umicom-framework-probe",kernel,config
    }
    ;
    OiPrint(t,"UMICOM_OS_BUILT\t1\nsource\t%s\n",p->sourceId);
    for(size_t i=0;i<5&&s==UMI_STATUS_OK;++i) {
        unsigned char *data=NULL;
        size_t size=0;
        char hash[65];
        s=OiReadJoined(root,files[i],UMI_OS_IMAGE_MAX_FILE,&data,&size);
        if(s==UMI_STATUS_OK&&i<3)s=UmiOsImageValidateElf(data,size,p->arch);
        if(s==UMI_STATUS_OK&&i==3)s=UmiOsImageValidateKernel(data,size,p->arch);
        if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(data,size,hash);
        if(s==UMI_STATUS_OK)OiPrint(t,"file\t%s\t%zu\t%s\n",hash,size,files[i]);
        free(data);
    }
    /* The source archive is checked through the existing streaming Framework
     * digest service. Buildroot's own package hashes remain enabled as well. */
    if(s==UMI_STATUS_OK) {
        char relative[UMI_OS_IMAGE_NAME],path[UMI_OS_IMAGE_PATH],hash[65];
        uint64_t bytes=0;
        int n=snprintf(relative,sizeof relative,"downloads/linux/linux-%s.tar.xz",p->linuxVersion);
        if(n<0||(size_t)n>=sizeof relative)s=UMI_STATUS_CAPACITY_EXCEEDED;
        if(s==UMI_STATUS_OK)s=OiJoin(root,relative,path);
        if(s==UMI_STATUS_OK)s=UmiNativeStageDigestFile(path,hash,&bytes,NULL);
        if(s==UMI_STATUS_OK&&strcmp(hash,p->linuxHash))s=UMI_STATUS_INVALID_STATE;
        if(s==UMI_STATUS_OK)OiPrint(t,"file\t%s\t%" PRIu64 "\t%s\n",hash,bytes,relative);
    }
    return s==UMI_STATUS_OK?t->status:s;
}
UmiStatus OiBuildReady(const char *root,const OiPlan *p,OiText *evidence) {
    OiText text;
    OiTextInit(&text);
    UmiStatus s=ConfigStamp(root,p,&text);
    if(s==UMI_STATUS_OK)s=Matches(root,"configured.umi",&text);
    OiTextClear(&text);
    if(s==UMI_STATUS_OK)s=BuildStamp(root,p,&text);
    if(s==UMI_STATUS_OK)s=Matches(root,"built.umi",&text);
    if(s==UMI_STATUS_OK&&evidence) {
        OiAppend(evidence,text.data,text.size);
        s=evidence->status;
    }
    OiTextClear(&text);
    return s;
}
static UmiStatus Attempt(const char *root,const char *kind,char out[UMI_OS_IMAGE_PATH]) {
    for(unsigned i=1;i<=1024U;++i) {
        char name[80];
        int n=snprintf(name,sizeof name,"%s-attempt-%04u",kind,i);
        if(n<0||(size_t)n>=sizeof name)return UMI_STATUS_CAPACITY_EXCEEDED;
        UmiStatus s=OiJoin(root,name,out);
        if(s!=UMI_STATUS_OK)return s;
        s=OiDirectory(out,1);
        if(s!=UMI_STATUS_ALREADY_EXISTS)return s;
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}
static UmiStatus Run(const char *root,const char *make,const char *kind,unsigned jobs, const UmiCancellationToken *cancel,UmiOsImageReport *r) {
    OiInit(r);
    if(!OiNormalLinuxUser())return OiReport(r,UMI_STATUS_PERMISSION_DENIED,"Run the Buildroot steps as a normal Linux/WSL user, not root.");
    if(OiAbsolute(root,1)!=UMI_STATUS_OK||OiAbsolute(make,1)!=UMI_STATUS_OK||jobs<1U||jobs>64U)return UMI_STATUS_INVALID_ARGUMENT;
    void *lock=NULL;
    UmiStatus locked=OiLock(root,&lock);
    if(locked!=UMI_STATUS_OK)return OiReport(r,locked,"Another native image operation owns this workspace, or its lock is invalid.");
    OiPlan *p=calloc(1,sizeof *p);
    if(!p) {
        OiUnlock(lock);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus s=OiPlanLoad(root,p);
    if(s==UMI_STATUS_OK)s=OiBuildroot(p);
    int configure=!strcmp(kind,"configure"),build=!strcmp(kind,"build");
    unsigned char *old=NULL;
    size_t oldSize=0;
    if(s==UMI_STATUS_OK&&(configure||build)) {
        UmiStatus found=OiReadJoined(root,configure?"configured.umi":"built.umi",OI_META_LIMIT,&old,&oldSize);
        free(old);
        if(found!=UMI_STATUS_NOT_FOUND)s=found==UMI_STATUS_OK?UMI_STATUS_ALREADY_EXISTS:found;
    }
    OiText stamp;
    OiTextInit(&stamp);
    if(s==UMI_STATUS_OK&&!configure) {
        s=ConfigStamp(root,p,&stamp);
        if(s==UMI_STATUS_OK)s=Matches(root,"configured.umi",&stamp);
        OiTextClear(&stamp);
    }
    char buildRoot[UMI_OS_IMAGE_PATH],external[UMI_OS_IMAGE_PATH],downloads[UMI_OS_IMAGE_PATH],o[UMI_OS_IMAGE_PATH+3],ext[UMI_OS_IMAGE_PATH+14],dl[UMI_OS_IMAGE_PATH+12],profile[80],parallel[40];
    if(s==UMI_STATUS_OK)s=OiJoin(root,"build",buildRoot);
    if(s==UMI_STATUS_OK&&configure)s=OiDirectory(buildRoot,1);
    if(s==UMI_STATUS_OK)s=OiJoin(root,"inputs/umicomOS/image",external);
    if(s==UMI_STATUS_OK)s=OiJoin(root,"downloads",downloads);
    if(s==UMI_STATUS_OK) {
        int a=snprintf(o,sizeof o,"O=%s",buildRoot),b=snprintf(ext,sizeof ext,"BR2_EXTERNAL=%s",external), c=snprintf(profile,sizeof profile,"umicom_%s_defconfig",OiArchName(p->arch)),d=snprintf(parallel,sizeof parallel,"BR2_JLEVEL=%u",jobs),e=snprintf(dl,sizeof dl,"BR2_DL_DIR=%s",downloads);
        if(a<0||b<0||c<0||d<0||e<0||(size_t)e>=sizeof dl||(size_t)a>=sizeof o||(size_t)b>=sizeof ext||(size_t)c>=sizeof profile||(size_t)d>=sizeof parallel)s=UMI_STATUS_CAPACITY_EXCEEDED;
    }
    char attempt[UMI_OS_IMAGE_PATH]= {
        0
    }
    ;
    if(s==UMI_STATUS_OK)s=Attempt(root,kind,attempt);
    if(s==UMI_STATUS_OK) {
        OiText capture;
        OiTextInit(&capture);
        UmiProcessResult *pr=calloc(1,sizeof *pr);
        if(!pr)s=UMI_STATUS_OUT_OF_MEMORY;
        else {
            const char *args[]= {
                "-C",p->buildroot,o,ext,dl,configure?profile:build?parallel:"legal-info"
            }
            ;
            s=OiCaptureLimited(make,args,6,root,14400000U,cancel,UMI_OS_IMAGE_MAX_BUILD_LOG,&capture,pr);
            if(r) {
                r->processLaunched=pr->launched;
                r->exitCode=pr->exit_code;
                r->timedOut=pr->timed_out;
                r->cancelled=pr->cancelled;
            }
            char path[UMI_OS_IMAGE_PATH];
            UmiStatus written=OiJoin(attempt,"output.log",path);
            if(written==UMI_STATUS_OK)written=OiWrite(path,capture.data,capture.size);
            if(s==UMI_STATUS_OK)s=written;
            OiText result;
            OiTextInit(&result);
            OiPrint(&result,"UMICOM_OS_COMMAND\t1\nsource\t%s\nkind\t%s\nlaunched\t%d\nexit\t%d\nstatus\t%d\n",p->sourceId,kind,pr->launched,pr->exit_code,(int)s);
            if(result.status==UMI_STATUS_OK&&OiJoin(attempt,"result.umi",path)==UMI_STATUS_OK) {
                UmiStatus saved=OiWrite(path,result.data,result.size);
                if(s==UMI_STATUS_OK)s=saved;
            }
            OiTextClear(&result);
            free(pr);
        }
        OiTextClear(&capture);
    }
    if(s==UMI_STATUS_OK) {
        char expected[65];
        strcpy(expected,p->sourceId);
        s=OiPlanLoad(root,p);
        if(s==UMI_STATUS_OK&&strcmp(expected,p->sourceId))s=UMI_STATUS_INVALID_STATE;
        if(s==UMI_STATUS_OK)s=OiBuildroot(p);
    }
    if(s==UMI_STATUS_OK)s=ConfigStamp(root,p,&stamp);
    if(s==UMI_STATUS_OK&&!configure)s=Matches(root,"configured.umi",&stamp);
    if(s==UMI_STATUS_OK&&build) {
        OiTextClear(&stamp);
        s=BuildStamp(root,p,&stamp);
    }
    if(s==UMI_STATUS_OK&&(configure||build)) {
        char path[UMI_OS_IMAGE_PATH];
        s=OiJoin(root,configure?"configured.umi":"built.umi",path);
        if(s==UMI_STATUS_OK)s=OiWrite(path,stamp.data,stamp.size);
    }
    if(s==UMI_STATUS_OK&&r) {
        r->completed=1;
        strcpy(r->sourceId,p->sourceId);
    }
    OiTextClear(&stamp);
    free(p);
    OiUnlock(lock);
    return OiReport(r,s,s==UMI_STATUS_OK?"Native build stage completed. No guest boot is implied.":"Build stage stopped. Retained logs and output are evidence, not a successful image.");
}
UmiStatus UmiOsImageConfigure(const char *r,const char *m,const UmiCancellationToken *c,UmiOsImageReport *o) {
    return Run(r,m,"configure",2,c,o);
}
UmiStatus UmiOsImageBuild(const char *r,const char *m,unsigned j,const UmiCancellationToken *c,UmiOsImageReport *o) {
    return Run(r,m,"build",j,c,o);
}
UmiStatus UmiOsImageLegalInfo(const char *r,const char *m,const UmiCancellationToken *c,UmiOsImageReport *o) {
    return Run(r,m,"legal-info",2,c,o);
}
