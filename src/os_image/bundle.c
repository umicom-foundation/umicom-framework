/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/bundle.c
 * PURPOSE:
 *   Assemble and verify the fixed diskless profile. A bundle is immutable input; separate
 *   boot-result directories record executions without mutating it.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Assemble and verify the fixed diskless profile. A bundle is immutable input;
 * separate boot-result directories record executions without mutating it. */
#include "internal.h"
typedef struct Rule {
    const char *name;
    uint32_t mode,major,minor;
}
Rule;
static const Rule Rules[]= {
    {
        "dev",OI_DIR|0755U,0,0
    }
    , {
        "dev/console",OI_CHR|0600U,5,1
    }
    , {
        "dev/null",OI_CHR|0666U,1,3
    }
    , {
        "etc",OI_DIR|0755U,0,0
    }
    , {
        "etc/group",OI_REG|0444U,0,0
    }
    , {
        "etc/os-release",OI_REG|0444U,0,0
    }
    , {
        "etc/passwd",OI_REG|0444U,0,0
    }
    , {
        "etc/shadow",OI_REG|0400U,0,0
    }
    , {
        "etc/umicom",OI_DIR|0755U,0,0
    }
    , {
        "etc/umicom/boot.conf",OI_REG|0444U,0,0
    }
    , {
        "etc/umicom/source-id",OI_REG|0444U,0,0
    }
    , {
        "init",OI_REG|0755U,0,0
    }
    , {
        "proc",OI_DIR|0555U,0,0
    }
    , {
        "root",OI_DIR|0700U,0,0
    }
    , {
        "run",OI_DIR|0755U,0,0
    }
    , {
        "sys",OI_DIR|0555U,0,0
    }
    , {
        "tmp",OI_DIR|01777U,0,0
    }
    , {
        "usr",OI_DIR|0755U,0,0
    }
    , {
        "usr/libexec",OI_DIR|0755U,0,0
    }
    , {
        "usr/libexec/umicom-framework-probe",OI_REG|0755U,0,0
    }
    , {
        "usr/libexec/umicom-platform-check",OI_REG|0755U,0,0
    }
    , {
        "usr/share",OI_DIR|0755U,0,0
    }
    , {
        "usr/share/umicom",OI_DIR|0755U,0,0
    }
    , {
        "usr/share/umicom/packages.json",OI_REG|0444U,0,0
    }
}
;
static int Program(const char *name) {
    return !strcmp(name,"init")||!strcmp(name,"usr/libexec/umicom-platform-check")||!strcmp(name,"usr/libexec/umicom-framework-probe");
}
static const UmiOsImageEntry *Find(const UmiOsImageEntry *entries,size_t count,const char *name) {
    for(size_t i=0;i<count;++i)if(!strcmp(entries[i].name,name))return &entries[i];
    return NULL;
}
static UmiStatus Packages(const UmiOsImageEntry *e,size_t count,const OiPlan *p,OiText *out) {
    const char *names[]= {
        "init","usr/libexec/umicom-platform-check","usr/libexec/umicom-framework-probe"
    }
    ;
    OiPrint(out,"{\n  \"profile\": \"diskless-foundation\",\n  \"architecture\": \"%s\",\n  \"kernel_request\": \"%s\",\n  \"buildroot_commit\": \"%s\",\n  \"libc_request\": \"static-musl\",\n  \"programs\": {\n",OiArchName(p->arch),p->linuxVersion,p->commit);
    for(size_t i=0;i<3;++i) {
        const UmiOsImageEntry *f=Find(e,count,names[i]);
        char hash[65];
        if(!f)return UMI_STATUS_INVALID_STATE;
        UmiStatus s=UmiNativeSha256Buffer(f->data,f->size,hash);
        if(s!=UMI_STATUS_OK)return s;
        OiPrint(out,"    \"%s\": {\"size\": %zu, \"sha256\": \"%s\"}%s\n",names[i],f->size,hash,i==2U?"":",");
    }
    OiPrint(out,"  },\n  \"notice\": \"No GUI, persistent storage, login shell or network service. Build requests are not boot evidence.\"\n}\n");
    return out->status;
}
UmiStatus OiRootfsCheck(UmiOsImageArchive *archive,const OiBundle *b,const OiPlan *p) {
    size_t count=UmiOsImageArchiveCount(archive);
    if(count!=sizeof Rules/sizeof Rules[0])return UMI_STATUS_INVALID_STATE;
    UmiOsImageEntry entries[sizeof Rules/sizeof Rules[0]];
    for(size_t i=0;i<count;++i) {
        const UmiOsImageEntry *e=UmiOsImageArchiveAt(archive,i);
        entries[i]=*e;
        const Rule *rule=&Rules[i];
        if(strcmp(e->name,rule->name)||e->mode!=rule->mode||e->major!=rule->major||e->minor!=rule->minor)return UMI_STATUS_INVALID_STATE;
        if(Program(e->name)) {
            UmiStatus s=UmiOsImageValidateElf(e->data,e->size,p->arch);
            if(s!=UMI_STATUS_OK)return s;
        }
        else if(!strcmp(e->name,"etc/umicom/source-id")) {
            if(e->size!=65U||memcmp(e->data,b->sourceId,64)||e->data[64]!='\n')return UMI_STATUS_INVALID_STATE;
        }
        else if((e->mode&OI_TYPE_MASK)==OI_REG&&strcmp(e->name,"usr/share/umicom/packages.json")) {
            char name[UMI_OS_IMAGE_NAME],hash[65];
            int n=snprintf(name,sizeof name,"umicomOS/rootfs/foundation/%s",e->name);
            if(n<0||(size_t)n>=sizeof name)return UMI_STATUS_CAPACITY_EXCEEDED;
            if(UmiNativeSha256Buffer(e->data,e->size,hash)!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;
            int found=0;
            for(size_t j=0;j<p->count;++j)if(!strcmp(p->inputs[j].name,name)&&p->inputs[j].size==e->size&&!strcmp(p->inputs[j].hash,hash))found=1;
            if(!found)return UMI_STATUS_INVALID_STATE;
        }
    }
    OiText expected;
    OiTextInit(&expected);
    UmiStatus s=Packages(entries,count,p,&expected);
    const UmiOsImageEntry *actual=Find(entries,count,"usr/share/umicom/packages.json");
    if(s==UMI_STATUS_OK&&(!actual||actual->size!=expected.size||memcmp(actual->data,expected.data,expected.size)))s=UMI_STATUS_INVALID_STATE;
    OiTextClear(&expected);
    return s;
}
/* The bytes copied into the archive must be the bytes admitted by the build
 * marker, not merely another structurally valid file read later at that path. */
UmiStatus OiCapturedArtifact(const OiText *evidence,const char *relative,const void *data,size_t size) {
    if(!evidence||evidence->status!=UMI_STATUS_OK||!evidence->data)return UMI_STATUS_INVALID_ARGUMENT;
    char hash[65];
    UmiStatus s=UmiNativeSha256Buffer(data,size,hash);
    OiText expected;
    OiTextInit(&expected);
    if(s==UMI_STATUS_OK) {
        OiPrint(&expected,"file\t%s\t%zu\t%s\n",hash,size,relative);
        s=expected.status;
    }
    int found=0;
    if(s==UMI_STATUS_OK) {
        const char *line=(const char*)evidence->data;
        while(*line) {
            const char *end=strchr(line,'\n');
            if(!end)break;
            size_t n=(size_t)(end-line)+1U;
            if(n==expected.size&&!memcmp(line,expected.data,n))++found;
            line=end+1;
        }
    }
    OiTextClear(&expected);
    return s!=UMI_STATUS_OK?s:found==1?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}
UmiStatus OiPackFiles(const char *target,const char *overlay,const char *kernel,const unsigned char *manifest, size_t manifestSize,const OiPlan *p,const OiText *evidence,const char *dest,UmiOsImageReport *r) {
    UmiOsImageEntry entries[sizeof Rules/sizeof Rules[0]];
    unsigned char *owned[sizeof Rules/sizeof Rules[0]]= {
        0
    }
    ;
    const size_t count=sizeof Rules/sizeof Rules[0];
    memset(entries,0,sizeof entries);
    UmiStatus s=UMI_STATUS_OK;
    char source[66];
    memcpy(source,p->sourceId,64);
    source[64]='\n';
    source[65]=0;
    uint64_t total=0;
    for(size_t i=0;i<count&&s==UMI_STATUS_OK;++i) {
        const Rule *rule=&Rules[i];
        UmiOsImageEntry *e=&entries[i];
        *e=(UmiOsImageEntry) {
            rule->name,rule->mode,rule->major,rule->minor,NULL,0
        }
        ;
        if(!strcmp(e->name,"etc/umicom/source-id")) {
            e->data=(const unsigned char*)source;
            e->size=65;
        }
        else if((e->mode&OI_TYPE_MASK)==OI_REG&&strcmp(e->name,"usr/share/umicom/packages.json")) {
            s=OiReadJoined(Program(e->name)?target:overlay,e->name,UMI_OS_IMAGE_MAX_FILE,&owned[i],&e->size);
            e->data=owned[i];
            if(s==UMI_STATUS_OK&&Program(e->name)) {
                s=UmiOsImageValidateElf(e->data,e->size,p->arch);
                char name[UMI_OS_IMAGE_NAME];
                int n=snprintf(name,sizeof name,"build/target/%s",e->name);
                if(n<0||(size_t)n>=sizeof name)s=UMI_STATUS_CAPACITY_EXCEEDED;
                if(s==UMI_STATUS_OK)s=OiCapturedArtifact(evidence,name,e->data,e->size);
            }
            if(s==UMI_STATUS_OK) {
                total+=e->size;
                if(total>UMI_OS_IMAGE_MAX_ARCHIVE-4096U)s=UMI_STATUS_CAPACITY_EXCEEDED;
            }
        }
    }
    OiText packages;
    OiTextInit(&packages);
    if(s==UMI_STATUS_OK)s=Packages(entries,count,p,&packages);
    entries[count-1U].data=packages.data;
    entries[count-1U].size=packages.size;
    unsigned char *kernelData=NULL,*archiveData=NULL;
    size_t kernelSize=0,archiveSize=0;
    if(s==UMI_STATUS_OK)s=OiRead(kernel,UMI_OS_IMAGE_MAX_FILE,&kernelData,&kernelSize);
    if(s==UMI_STATUS_OK)s=UmiOsImageValidateKernel(kernelData,kernelSize,p->arch);
    if(s==UMI_STATUS_OK) {
        char name[UMI_OS_IMAGE_NAME];
        int n=snprintf(name,sizeof name,"build/images/%s",OiKernelName(p->arch));
        if(n<0||(size_t)n>=sizeof name)s=UMI_STATUS_CAPACITY_EXCEEDED;
        if(s==UMI_STATUS_OK)s=OiCapturedArtifact(evidence,name,kernelData,kernelSize);
    }
    if(s==UMI_STATUS_OK)s=UmiOsImageArchiveBuild(entries,count,&archiveData,&archiveSize);
    UmiOsImageArchive *archive=NULL;
    OiBundle b= {
        0
    }
    ;
    b.arch=p->arch;
    strcpy(b.sourceId,p->sourceId);
    if(s==UMI_STATUS_OK)s=UmiOsImageArchiveOpen(archiveData,archiveSize,&archive);
    if(s==UMI_STATUS_OK)s=OiRootfsCheck(archive,&b,p);
    UmiOsImageArchiveDestroy(archive);
    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(kernelData,kernelSize,b.kernelHash);
    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(archiveData,archiveSize,b.archiveHash);
    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(manifest,manifestSize,b.manifestHash);
    if(s==UMI_STATUS_OK&&strcmp(b.manifestHash,p->sourceId))s=UMI_STATUS_INVALID_STATE;
    OiText record;
    OiTextInit(&record);
    if(s==UMI_STATUS_OK) {
        OiPrint(&record,"UMICOM_OS_IMAGE\t1\narch\t%s\nsource\t%s\nkernel\t%zu\t%s\nrootfs\t%zu\t%s\ninputs\t%zu\t%s\nboot\tnot-run\n", OiArchName(p->arch),p->sourceId,kernelSize,b.kernelHash,archiveSize,b.archiveHash,manifestSize,b.manifestHash);
        s=record.status;
    }
    if(s==UMI_STATUS_OK)s=OiDirectory(dest,1);
    if(s==UMI_STATUS_OK&&r)r->outputCreated=1;
    const char *names[]= {
        OiKernelName(p->arch),"umicom-rootfs.cpio.gz","input-manifest.umi","image.umi"
    }
    ;
    const unsigned char *data[]= {
        kernelData,archiveData,manifest,record.data
    }
    ;
    const size_t sizes[]= {
        kernelSize,archiveSize,manifestSize,record.size
    }
    ;
    for(size_t i=0;i<4U&&s==UMI_STATUS_OK;++i) {
        char path[UMI_OS_IMAGE_PATH];
        s=OiJoin(dest,names[i],path);
        if(s==UMI_STATUS_OK)s=OiWrite(path,data[i],sizes[i]);
        if(s==UMI_STATUS_OK&&r) {
            ++r->files;
            r->bytes+=sizes[i];
        }
    }
    if(s==UMI_STATUS_OK&&r) {
        r->completed=1;
        strcpy(r->sourceId,p->sourceId);
    }
    OiTextClear(&record);
    OiTextClear(&packages);
    free(kernelData);
    free(archiveData);
    for(size_t i=0;i<count;++i)free(owned[i]);
    return s;
}
static char *Next(char **at) {
    if(!**at)return NULL;
    char *p=*at,*end=strchr(p,'\n');
    if(!end)return NULL;
    *end=0;
    *at=end+1;
    return p;
}
static char *Field(char **at,const char *name) {
    char *p=Next(at);
    size_t n=strlen(name);
    return p&&!strncmp(p,name,n)&&p[n]=='\t'?p+n+1U:NULL;
}
static int FileField(char **at,const char *key,uint64_t *bytes,char hash[65]) {
    char *v=Field(at,key);
    if(!v)return 0;
    char *tab=strchr(v,'\t');
    if(!tab)return 0;
    *tab++=0;
    if(!OiNumber(v,bytes)||!OiHash(tab,64))return 0;
    strcpy(hash,tab);
    return 1;
}
UmiStatus OiBundleLoad(const char *root,OiBundle *b) {
    unsigned char *data=NULL;
    size_t size=0;
    UmiStatus s=OiReadJoined(root,"image.umi",4096,&data,&size);
    if(s!=UMI_STATUS_OK)return s;
    s=UMI_STATUS_PARSE_ERROR;
    if(!size||data[size-1U]!='\n'||memchr(data,0,size))goto end;
    char *at=(char*)data,*v=Next(&at);
    if(!v||strcmp(v,"UMICOM_OS_IMAGE\t1"))goto end;
    v=Field(&at,"arch");
    if(!v)goto end;
    if(!strcmp(v,"riscv64"))b->arch=UMI_OS_IMAGE_RISCV64;
    else if(!strcmp(v,"x86_64"))b->arch=UMI_OS_IMAGE_X86_64;
    else goto end;
    v=Field(&at,"source");
    if(!OiHash(v,64))goto end;
    strcpy(b->sourceId,v);
    if(!FileField(&at,"kernel",&b->kernelSize,b->kernelHash)||!FileField(&at,"rootfs",&b->archiveSize,b->archiveHash)|| !FileField(&at,"inputs",&b->manifestSize,b->manifestHash))goto end;
    v=Field(&at,"boot");
    if(!v||strcmp(v,"not-run")||*at||b->kernelSize>UMI_OS_IMAGE_MAX_FILE||b->archiveSize>UMI_OS_IMAGE_MAX_ARCHIVE+16384U|| b->manifestSize>OI_META_LIMIT||strcmp(b->sourceId,b->manifestHash))goto end;
    s=UMI_STATUS_OK;
    end:free(data);
    return s;
}
UmiStatus UmiOsImageVerify(const char *root,UmiOsImageReport *r) {
    OiInit(r);
    if(OiAbsolute(root,0)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    OiBundle b= {
        0
    }
    ;
    UmiStatus s=OiBundleLoad(root,&b);
    OiPlan *p=calloc(1,sizeof *p);
    if(!p)return UMI_STATUS_OUT_OF_MEMORY;
    const char *names[]= {
        OiKernelName(b.arch),"umicom-rootfs.cpio.gz","input-manifest.umi","image.umi"
    }
    ;
    if(s==UMI_STATUS_OK)s=OiInventory(root,names,4);
    const char *hashes[]= {
        b.kernelHash,b.archiveHash,b.manifestHash
    }
    ;
    uint64_t lengths[]= {
        b.kernelSize,b.archiveSize,b.manifestSize
    }
    ;
    unsigned char *data[3]= {
        0
    }
    ;
    size_t sizes[3]= {
        0
    }
    ;
    for(size_t i=0;i<3U&&s==UMI_STATUS_OK;++i) {
        s=OiReadJoined(root,names[i],UMI_OS_IMAGE_MAX_ARCHIVE+16384U,&data[i],&sizes[i]);
        char hash[65];
        if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(data[i],sizes[i],hash);
        if(s==UMI_STATUS_OK&&(sizes[i]!=lengths[i]||strcmp(hash,hashes[i])))s=UMI_STATUS_INVALID_STATE;
    }
    if(s==UMI_STATUS_OK)s=UmiOsImageValidateKernel(data[0],sizes[0],b.arch);
    if(s==UMI_STATUS_OK)s=OiPlanDecode(data[2],sizes[2],p);
    if(s==UMI_STATUS_OK&&(p->arch!=b.arch||strcmp(p->sourceId,b.sourceId)))s=UMI_STATUS_INVALID_STATE;
    UmiOsImageArchive *a=NULL;
    if(s==UMI_STATUS_OK)s=UmiOsImageArchiveOpen(data[1],sizes[1],&a);
    if(s==UMI_STATUS_OK)s=OiRootfsCheck(a,&b,p);
    UmiOsImageArchiveDestroy(a);
    if(s==UMI_STATUS_OK&&r) {
        r->completed=1;
        r->files=4;
        strcpy(r->sourceId,b.sourceId);
        r->bytes=b.kernelSize+b.archiveSize+b.manifestSize;
    }
    free(p);
    for(size_t i=0;i<3;++i)free(data[i]);
    return OiReport(r,s,s==UMI_STATUS_OK?"Bundle bytes and diskless profile agree. Boot and authenticity are not tested.":"Bundle verification failed; no guest was started.");
}
UmiStatus UmiOsImagePack(const char *root,const char *dest,UmiOsImageReport *r) {
    OiInit(r);
    if(OiAbsolute(root,1)!=UMI_STATUS_OK||OiAbsolute(dest,0)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    void *lock=NULL;
    UmiStatus locked=OiLock(root,&lock);
    if(locked!=UMI_STATUS_OK)return locked;
    OiPlan *p=calloc(1,sizeof *p);
    if(!p) {
        OiUnlock(lock);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    UmiStatus s=OiPlanLoad(root,p);
    OiText evidence;
    OiTextInit(&evidence);
    if(s==UMI_STATUS_OK)s=OiBuildReady(root,p,&evidence);
    char target[UMI_OS_IMAGE_PATH],overlay[UMI_OS_IMAGE_PATH],kernel[UMI_OS_IMAGE_PATH],relative[80];
    int n=snprintf(relative,sizeof relative,"build/images/%s",OiKernelName(p->arch));
    if(s==UMI_STATUS_OK&&(n<0||(size_t)n>=sizeof relative))s=UMI_STATUS_CAPACITY_EXCEEDED;
    if(s==UMI_STATUS_OK)s=OiJoin(root,"build/target",target);
    if(s==UMI_STATUS_OK)s=OiJoin(root,"inputs/umicomOS/rootfs/foundation",overlay);
    if(s==UMI_STATUS_OK)s=OiJoin(root,relative,kernel);
    unsigned char *manifest=NULL;
    size_t size=0;
    if(s==UMI_STATUS_OK)s=OiReadJoined(root,"input-manifest.umi",OI_META_LIMIT,&manifest,&size);
    if(s==UMI_STATUS_OK)s=OiPackFiles(target,overlay,kernel,manifest,size,p,&evidence,dest,r);
    free(manifest);
    OiTextClear(&evidence);
    free(p);
    OiUnlock(lock);
    return OiReport(r,s,s==UMI_STATUS_OK?"Native diskless bundle written. Run verify, then separate normal and recovery boot tests.":"Packing stopped. Any new incomplete directory is retained; no old output was replaced.");
}
