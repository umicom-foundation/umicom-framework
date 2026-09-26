/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/catalogue.c
 * A bounded tab-delimited catalogue. Release paths cannot contain control
 * characters; therefore fields need no interpreter-dependent escaping.
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include <inttypes.h>
static char *Field(char **cursor)
{
    if(*cursor==NULL)return NULL;
    char *result=*cursor,*p=strchr(result,'\t');
    if(p) {
        *p=0;
        *cursor=p+1;
    }
    else *cursor=NULL;
    return result;
}

static int GetOwner(const UmiSetupBundle *b,const char *s,unsigned *owner)
{
    if(!strcmp(s,"shared")) {
        *owner=SC_SHARED;
        return 1;
    }
    for(size_t i=0; i<b->appCount; ++i)if(!strcmp(s,b->apps[i].id)) {
        *owner=(unsigned)i;
        return 1;
    }
    return 0;
}

UmiStatus ScDecode(const char *data,size_t length,UmiSetupBundle *b,int installed)
{

    if(!data||length>UMI_SETUP_MANIFEST_LIMIT||memchr(data,0,length))return UMI_STATUS_PARSE_ERROR;

    const char *header=installed?SC_RECEIPT_HEADER:SC_HEADER;
    size_t h=strlen(header);

    if(length<h||memcmp(data,header,h)||data[length-1U]!='\n')return UMI_STATUS_PARSE_ERROR;

    char *copy=malloc(length+1U);
    if(!copy)return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy,data,length);
    copy[length]=0;

    UmiStatus status=UMI_STATUS_OK;
    int seenFiles=0;
    uint64_t total=0;
    char *line=copy+h;

    while(*line) {
        char *end=strchr(line,'\n');
        if(!end) {
            status=UMI_STATUS_PARSE_ERROR;
            break;
        }
        *end=0;

        char *cursor=line,*kind=Field(&cursor);
        if(!kind) {
            status=UMI_STATUS_PARSE_ERROR;
            break;
        }

        if(!strcmp(kind,"app")) {

            char *id=Field(&cursor),*title=Field(&cursor),*entry=Field(&cursor);

            if(seenFiles||!id||!title||!entry||cursor||!ScToken(id)||!strcmp(id,"shared")||!ScUtf8(title)||!*title||strlen(title)>=192U||UmiSetupValidateRelative(entry)!=UMI_STATUS_OK||b->appCount==UMI_SETUP_MAX_APPS) {
                status=UMI_STATUS_PARSE_ERROR;
                break;
            }

            for(size_t i=0; i<b->appCount; ++i)if(!strcmp(id,b->apps[i].id)||ScEqualFold(entry,b->apps[i].entry)==0) {
                status=UMI_STATUS_PARSE_ERROR;
                break;
            }

            if(status!=UMI_STATUS_OK)break;

            UmiSetupApplication *a=&b->apps[b->appCount++];
            strcpy(a->id,id);
            strcpy(a->title,title);
            strcpy(a->entry,entry);

        }
        else if(!strcmp(kind,"file")) {

            seenFiles=1;
            char *owner=Field(&cursor),*size=Field(&cursor),*hash=Field(&cursor),*rel=Field(&cursor);
            ScFile f= {
                0
            };

            if(!owner||!size||!hash||!rel||cursor||!GetOwner(b,owner,&f.owner)||!ScNumber(size,&f.bytes)||!ScHashValid(hash)||UmiSetupValidateRelative(rel)!=UMI_STATUS_OK) {
                status=UMI_STATUS_PARSE_ERROR;
                break;
            }

            if(ScEqualFold(rel,UMI_SETUP_RECEIPT)==0||ScEqualFold(rel,UMI_SETUP_CATALOGUE)==0) {
                status=UMI_STATUS_PARSE_ERROR;
                break;
            }

            strcpy(f.relative,rel);
            strcpy(f.hash,hash);

            if(total>UMI_SETUP_MAX_TOTAL_BYTES||f.bytes>UMI_SETUP_MAX_TOTAL_BYTES-total) {
                status=UMI_STATUS_CAPACITY_EXCEEDED;
                break;
            }
            total+=f.bytes;

            status=ScAddFile(b,&f);
            if(status!=UMI_STATUS_OK) {
                status=UMI_STATUS_PARSE_ERROR;
                break;
            }

        }
        else {
            status=UMI_STATUS_PARSE_ERROR;
            break;
        }
        line=end+1;

    }

    if(status==UMI_STATUS_OK) {
        if(!b->appCount||!b->fileCount)status=UMI_STATUS_PARSE_ERROR;

        for(size_t i=0; i<b->appCount&&status==UMI_STATUS_OK; ++i) {
            int found=0;
            for(size_t j=0; j<b->fileCount; ++j)if(!strcmp(b->apps[i].entry,b->files[j].relative)&&b->files[j].owner==i) {
                found=1;
                break;
            }
            if(!found)status=UMI_STATUS_PARSE_ERROR;
        }
    }

    free(copy);
    b->installed=installed;
    return status;

}

UmiStatus ScEncode(const UmiSetupBundle *b,uint64_t selected,int installed,ScText *t)
{

    ScPrint(t,"%s",installed?SC_RECEIPT_HEADER:SC_HEADER);

    for(size_t i=0; i<b->appCount; ++i)if((selected>>i)&UINT64_C(1))ScPrint(t,"app\t%s\t%s\t%s\n",b->apps[i].id,b->apps[i].title,b->apps[i].entry);

    for(size_t i=0; i<b->fileCount; ++i) {
        const ScFile *f=&b->files[i];
        if(ScSelected(f,selected))ScPrint(t,"file\t%s\t%" PRIu64 "\t%s\t%s\n",f->owner==SC_SHARED?"shared":b->apps[f->owner].id,f->bytes,f->hash,f->relative);
    }

    return t->status;

}

void UmiSetupBundleDestroy(UmiSetupBundle *b) {
    if(b) {
        free(b->files);
        free(b);
    }
}

size_t UmiSetupApplicationCount(const UmiSetupBundle *b) {
    return b?b->appCount:0U;
}

const UmiSetupApplication *UmiSetupApplicationAt(const UmiSetupBundle *b,size_t i) {
    return b&&i<b->appCount?&b->apps[i]:NULL;
}

uint64_t UmiSetupAllApplications(const UmiSetupBundle *b) {
    return !b||!b->appCount?0:b->appCount==64U?UINT64_MAX:(UINT64_C(1)<<b->appCount)-1U;
}

static UmiStatus Open(const char *root,UmiSetupBundle **out,UmiSetupReport *r,int installed)
{

    if(r)memset(r,0,sizeof *r);
    if(!out)return UMI_STATUS_INVALID_ARGUMENT;
    *out=NULL;

    char path[UMI_SETUP_PATH_CAPACITY];
    UmiStatus status=ScJoin(root,installed?UMI_SETUP_RECEIPT:UMI_SETUP_CATALOGUE,path);
    if(status!=UMI_STATUS_OK)return status;

    char *data=NULL;
    size_t length=0;
    status=ScRead(path,UMI_SETUP_MANIFEST_LIMIT,&data,&length,r);
    if(status!=UMI_STATUS_OK)return status;

    UmiSetupBundle *b=calloc(1,sizeof *b);
    if(!b) {
        free(data);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    strcpy(b->root,root);

    status=ScDecode(data,length,b,installed);
    if(status==UMI_STATUS_OK)status=UmiNativeSha256Buffer(data,length,b->catalogueHash);
    free(data);

    if(status!=UMI_STATUS_OK) {
        UmiSetupBundleDestroy(b);
        ScReport(r,status,"The catalogue is malformed or unsupported.");
        return status;
    }
    *out=b;
    ScReport(r,UMI_STATUS_OK,"Catalogue loaded. No installation has started.");
    return UMI_STATUS_OK;

}

UmiStatus UmiSetupBundleOpen(const char *root,UmiSetupBundle **out,UmiSetupReport *r) {
    return Open(root,out,r,0);
}

UmiStatus UmiSetupVerifyInstallation(const char *root,UmiSetupReport *r)
{

    UmiSetupBundle *b=NULL;
    UmiStatus s=Open(root,&b,r,1);
    if(s!=UMI_STATUS_OK)return s;

    for(size_t i=0; i<b->fileCount&&s==UMI_STATUS_OK; ++i) {
        char path[UMI_SETUP_PATH_CAPACITY],hash[65];
        uint64_t bytes=0;
        s=ScJoin(root,b->files[i].relative,path);

        if(s==UMI_STATUS_OK)s=ScDigest(path,hash,&bytes,NULL,NULL,r);

        if(s==UMI_STATUS_OK&&(strcmp(hash,b->files[i].hash)||bytes!=b->files[i].bytes))s=UMI_STATUS_INVALID_STATE;

        if(s==UMI_STATUS_OK&&r)++r->filesCompleted;
    }

    UmiSetupBundleDestroy(b);
    if(r)r->completed=s==UMI_STATUS_OK;
    ScReport(r,s,s==UMI_STATUS_OK?"Installed files agree with their receipt. No application was launched.":"An installed file is missing, unreadable or changed.");
    return s;

}
