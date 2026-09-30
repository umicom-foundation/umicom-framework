/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/os_image/prepare.c
 * PURPOSE:
 *   Snapshot selected native sources without recursively copying repositories. Each file is
 *   read once and those same bytes are hashed and written. The input manifest is the final
 *   completion boundary, not a speculative build result.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Snapshot selected native sources without recursively copying repositories.
 * Each file is read once and those same bytes are hashed and written. The input
 * manifest is the final completion boundary, not a speculative build result. */
#include "internal.h"
UmiStatus UmiOsImagePrepare(const char *os,const char *fw,const char *br,const char *dest, UmiOsImageArch arch,const char *git,UmiOsImageReport *r) {
    OiInit(r);
#ifndef __linux__
    (void)os;
    (void)fw;
    (void)br;
    (void)dest;
    (void)arch;
    (void)git;
    return OiReport(r,UMI_STATUS_UNAVAILABLE,"Source preparation requires Linux or WSL.");
#else
    if(!OiArchValid(arch)||OiAbsolute(os,1)!=UMI_STATUS_OK||OiAbsolute(fw,1)!=UMI_STATUS_OK|| OiAbsolute(br,1)!=UMI_STATUS_OK||OiAbsolute(dest,1)!=UMI_STATUS_OK||OiAbsolute(git,1)!=UMI_STATUS_OK) return OiReport(r,UMI_STATUS_INVALID_ARGUMENT,"Use absolute build paths without spaces, control characters or shell metacharacters.");
    const char *roots[]= {
        os,fw,br
    }
    ;
    for(size_t i=0;i<3;++i)if(OiContains(roots[i],dest)||OiContains(dest,roots[i]))return OiReport(r,UMI_STATUS_INVALID_ARGUMENT,"The output must be outside all source and Buildroot trees.");
    OiPlan *p=calloc(1,sizeof *p);
    if(!p)return UMI_STATUS_OUT_OF_MEMORY;
    p->arch=arch;
    strcpy(p->buildroot,br);
    strcpy(p->git,git);
    UmiStatus s=OiProfileLoad(os,p);
    if(s==UMI_STATUS_OK)s=OiBuildroot(p);
    /* Validate every source before creating output. Re-read and hash the exact
     * bytes captured below; a preflight is not authority to skip later checks. */
    for(size_t i=0;i<p->count&&s==UMI_STATUS_OK;++i) {
        char path[UMI_OS_IMAGE_PATH];
        const char *name=p->inputs[i].name;
        s=OiJoin(!strncmp(name,"framework/",10)?fw:os,name+(!strncmp(name,"framework/",10)?10:9),path);
        if(s==UMI_STATUS_OK)s=OiDigest(path,p->inputs[i].hash,&p->inputs[i].size);
    }
    if(s==UMI_STATUS_OK)s=OiDirectory(dest,1);
    if(s==UMI_STATUS_OK&&r)r->outputCreated=1;
    char inputs[UMI_OS_IMAGE_PATH];
    if(s==UMI_STATUS_OK)s=OiJoin(dest,"inputs",inputs);
    if(s==UMI_STATUS_OK)s=OiDirectory(inputs,1);
    uint64_t total=0;
    for(size_t i=0;i<p->count&&s==UMI_STATUS_OK;++i) {
        char path[UMI_OS_IMAGE_PATH],out[UMI_OS_IMAGE_PATH];
        const char *name=p->inputs[i].name;
        s=OiJoin(!strncmp(name,"framework/",10)?fw:os,name+(!strncmp(name,"framework/",10)?10:9),path);
        unsigned char *bytes=NULL;
        size_t n=0;
        if(s==UMI_STATUS_OK)s=OiRead(path,UMI_OS_IMAGE_MAX_FILE,&bytes,&n);
        char hash[65];
        if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(bytes,n,hash);
        if(s==UMI_STATUS_OK&&(n!=p->inputs[i].size||strcmp(hash,p->inputs[i].hash)))s=UMI_STATUS_INVALID_STATE;
        if(s==UMI_STATUS_OK&&(uint64_t)n>OI_INPUT_LIMIT-total)s=UMI_STATUS_CAPACITY_EXCEEDED;
        if(s==UMI_STATUS_OK) {
            total+=n;
            s=OiParents(inputs,name);
        }
        if(s==UMI_STATUS_OK)s=OiJoin(inputs,name,out);
        if(s==UMI_STATUS_OK)s=OiWrite(out,bytes,n);
        free(bytes);
        if(s==UMI_STATUS_OK&&r) {
            ++r->files;
            r->bytes=total;
        }
    }
    OiText manifest;
    OiTextInit(&manifest);
    if(s==UMI_STATUS_OK) {
        OiPrint(&manifest,"UMICOM_OS_INPUTS\t1\narch\t%s\nbuildroot\t%s\nlinux\t%s\nlinux-sha256\t%s\n",OiArchName(arch),p->commit,p->linuxVersion,p->linuxHash);
        for(size_t i=0;i<p->count;++i)OiPrint(&manifest,"file\t%s\t%" PRIu64 "\t%s\n",p->inputs[i].hash,p->inputs[i].size,p->inputs[i].name);
        s=manifest.status;
    }
    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(manifest.data,manifest.size,p->sourceId);
    if(s==UMI_STATUS_OK) {
        OiText location;
        OiTextInit(&location);
        OiPrint(&location,"UMICOM_OS_LOCATION\t1\nbuildroot\t%s\ngit\t%s\nsource\t%s\n",br,git,p->sourceId);
        char path[UMI_OS_IMAGE_PATH];
        s=location.status;
        if(s==UMI_STATUS_OK)s=OiJoin(dest,"location.umi",path);
        if(s==UMI_STATUS_OK)s=OiWrite(path,location.data,location.size);
        OiTextClear(&location);
    }
    if(s==UMI_STATUS_OK)s=OiBuildroot(p);
    if(s==UMI_STATUS_OK) {
        char path[UMI_OS_IMAGE_PATH];
        s=OiJoin(dest,"input-manifest.umi",path);
        if(s==UMI_STATUS_OK)s=OiWrite(path,manifest.data,manifest.size);
    }
    if(s==UMI_STATUS_OK&&r) {
        r->completed=1;
        strcpy(r->sourceId,p->sourceId);
    }
    OiTextClear(&manifest);
    free(p);
    return OiReport(r,s,s==UMI_STATUS_OK?"Native inputs captured. No kernel build or guest boot was attempted.":"Preparation failed. Existing paths are unchanged; any new partial output is retained.");
#endif
}
