/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/install.c
 * The completion receipt is a boundary, not a progress counter. No receipt is
 * issued until all selected payload bytes agree. Failed new trees are retained.
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <inttypes.h>
static UmiStatus PayloadPath(const UmiSetupBundle *b,const ScFile *f,char path[UMI_SETUP_PATH_CAPACITY])
{
    char payload[UMI_SETUP_PATH_CAPACITY];
    UmiStatus s=ScJoin(b->root,"payload",payload);
    return s==UMI_STATUS_OK?ScJoin(payload,f->relative,path):s;
}

static UmiStatus CatalogueUnchanged(const UmiSetupBundle *b,UmiSetupReport *r)
{
    char p[UMI_SETUP_PATH_CAPACITY],hash[65];
    uint64_t bytes;
    UmiStatus s=ScJoin(b->root,UMI_SETUP_CATALOGUE,p);
    if(s==UMI_STATUS_OK)s=ScDigest(p,hash,&bytes,NULL,NULL,r);
    if(s==UMI_STATUS_OK&&strcmp(hash,b->catalogueHash))s=UMI_STATUS_INVALID_STATE;
    return s;
}

static UmiStatus Fingerprint(const UmiSetupBundle *b,uint64_t selected,const char *dest,char hash[65])
{
    ScText t;
    ScTextInit(&t);
    ScPrint(&t,"Umicom installation plan\n%s\n%016" PRIx64 "\n%zu:%s\n",b->catalogueHash,selected,strlen(dest),dest);
    UmiStatus s=t.status;
    if(s==UMI_STATUS_OK)s=UmiNativeSha256Buffer(t.data,t.size,hash);
    ScTextFree(&t);
    return s;
}

UmiStatus UmiSetupReview(const UmiSetupBundle *b,uint64_t selected,const char *dest,char hash[65],UmiSetupProgress progress,void *context,UmiSetupReport *r)
{

    if(r)memset(r,0,sizeof *r);

    if(!b||b->installed||!hash||!selected||(selected&~UmiSetupAllApplications(b))||UmiSetupValidateAbsolute(dest)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;

    UmiStatus s=CatalogueUnchanged(b,r);
    if(s!=UMI_STATUS_OK) {
        ScReport(r,s,"The selected catalogue changed. Load it again before review.");
        return s;
    }

    uint64_t total=0;

    for(size_t i=0; i<b->fileCount; ++i)if(ScSelected(&b->files[i],selected)) {
        if(b->files[i].bytes>UMI_SETUP_MAX_TOTAL_BYTES-total)return UMI_STATUS_CAPACITY_EXCEEDED;
        total+=b->files[i].bytes;
    }

    if(r)r->bytesPlanned=total;

    for(size_t i=0; i<b->fileCount&&s==UMI_STATUS_OK; ++i) {
        const ScFile *f=&b->files[i];
        if(!ScSelected(f,selected))continue;

        char path[UMI_SETUP_PATH_CAPACITY],actual[65];
        uint64_t bytes=0;
        s=PayloadPath(b,f,path);
        ScReport(r,s,"Checking %s",f->relative);

        if(s==UMI_STATUS_OK)s=ScDigest(path,actual,&bytes,progress,context,r);

        if(s==UMI_STATUS_OK&&(bytes!=f->bytes||strcmp(actual,f->hash)))s=UMI_STATUS_INVALID_STATE;

        if(s==UMI_STATUS_OK&&r) {
            ++r->filesCompleted;
            r->bytesCompleted+=bytes;
        }
    }

    if(s==UMI_STATUS_OK)s=Fingerprint(b,selected,dest,hash);

    ScReport(r,s,s==UMI_STATUS_OK?"Review complete. No files have been installed.":"A selected payload file changed, is unavailable, or review was cancelled.");
    return s;

}

static UmiStatus SpaceFor(const char *dest,uint64_t bytes,UmiSetupReport *r)
{
    char parent[UMI_SETUP_PATH_CAPACITY];
    strcpy(parent,dest);
    char *last=strrchr(parent,'/'),*back=strrchr(parent,'\\');
    if(back&&(!last||back>last))last=back;
    if(!last)return UMI_STATUS_INVALID_ARGUMENT;
    if(last==parent)last[1]=0;
    else if(last==parent+2&&parent[1]==':')last[1]=0;
    else *last=0;
    return ScFreeSpace(parent,bytes,r);
}

UmiStatus UmiSetupInstall(const UmiSetupBundle *b,uint64_t selected,const char *dest,const char *expected,UmiSetupProgress progress,void *context,UmiSetupReport *r)
{

    if(!ScHashValid(expected))return UMI_STATUS_INVALID_ARGUMENT;

    UmiSetupReport local= {
        0
    };
    if(!r)r=&local;
    char hash[65];
    UmiStatus s=UmiSetupReview(b,selected,dest,hash,progress,context,r);

    if(s!=UMI_STATUS_OK)return s;
    if(strcmp(hash,expected)) {
        ScReport(r,UMI_STATUS_INVALID_STATE,"The selection or destination differs from the reviewed plan.");
        return UMI_STATUS_INVALID_STATE;
    }

    s=SpaceFor(dest,r->bytesPlanned,r);
    if(s!=UMI_STATUS_OK)return s;

    if(progress&&progress(r,context))return UMI_STATUS_CANCELLED;

    s=ScMakeDirectory(dest,r);
    if(s!=UMI_STATUS_OK)return s;
    r->outputCreated=1;
    r->bytesCompleted=0;
    r->filesCompleted=0;

    for(size_t i=0; i<b->fileCount&&s==UMI_STATUS_OK; ++i) {
        const ScFile *f=&b->files[i];
        if(!ScSelected(f,selected))continue;

        char source[UMI_SETUP_PATH_CAPACITY],target[UMI_SETUP_PATH_CAPACITY];
        s=PayloadPath(b,f,source);
        if(s==UMI_STATUS_OK)s=ScJoin(dest,f->relative,target);

        if(s==UMI_STATUS_OK)s=ScParents(dest,f->relative,r);
        ScReport(r,s,"Installing %s",f->relative);

        if(s==UMI_STATUS_OK)s=ScCopy(source,target,f->hash,f->bytes,progress,context,r);
        if(s==UMI_STATUS_OK)++r->filesCompleted;
    }

    if(s==UMI_STATUS_OK&&progress&&progress(r,context))s=UMI_STATUS_CANCELLED;

    if(s==UMI_STATUS_OK)s=CatalogueUnchanged(b,r);

    if(s==UMI_STATUS_OK) {
        ScText text;
        ScTextInit(&text);
        s=ScEncode(b,selected,1,&text);
        char receipt[UMI_SETUP_PATH_CAPACITY];
        if(s==UMI_STATUS_OK)s=ScJoin(dest,UMI_SETUP_RECEIPT,receipt);
        if(s==UMI_STATUS_OK)s=ScWriteNew(receipt,text.data,text.size,r);
        ScTextFree(&text);
    }

    r->completed=s==UMI_STATUS_OK;
    ScReport(r,s,s==UMI_STATUS_OK?"Selected applications installed. No application was started.":"Installation is incomplete. Keep the new directory for inspection; choose a different new destination for another attempt.");
    return s;

}
