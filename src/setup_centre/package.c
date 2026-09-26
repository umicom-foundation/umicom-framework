/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/setup_centre/package.c
 * Reuse existing runtime ownership reports, not broad build-directory scans.
 * The five generated CMake assignments are treated as a closed data format:
 * no include(), expansion, command, variable reference or CMake execution.
 *---------------------------------------------------------------------------*/

#include "internal.h"
static char *Next(char **p,char separator)
{
    if(!*p)return NULL;
    char *s=*p,*end=strchr(s,separator);
    if(end) {
        *end=0;
        *p=end+1;
    }
    else *p=NULL;
    return s;
}

static UmiStatus Assignment(char **cursor,const char *key,char **out)
{

    char prefix[128];
    int n=snprintf(prefix,sizeof prefix,"set(%s [==[",key);
    if(n<=0||(size_t)n>=sizeof prefix)return UMI_STATUS_INTERNAL_ERROR;

    char *p=*cursor;
    if(strncmp(p,prefix,(size_t)n))return UMI_STATUS_PARSE_ERROR;
    p+=(size_t)n;
    char *end=strstr(p,"]==])\n");
    if(!end)return UMI_STATUS_PARSE_ERROR;
    *end=0;
    *out=p;
    *cursor=end+6;
    return UMI_STATUS_OK;

}

static UmiStatus AddSource(UmiSetupBundle *b,const char *relative,const char *source,const char *expected,unsigned owner,UmiSetupReport *r)
{

    if(UmiSetupValidateRelative(relative)!=UMI_STATUS_OK||UmiSetupValidateAbsolute(source)!=UMI_STATUS_OK)return UMI_STATUS_PARSE_ERROR;

    ScFile f= {
        0
    };
    strcpy(f.relative,relative);
    strcpy(f.source,source);
    f.owner=owner;

    UmiStatus s=ScDigest(source,f.hash,&f.bytes,NULL,NULL,r);
    if(s!=UMI_STATUS_OK)return s;
    if(expected&&(!ScHashValid(expected)||strcmp(expected,f.hash)))return UMI_STATUS_INVALID_STATE;

    s=ScAddFile(b,&f);
    return s==UMI_STATUS_ALREADY_EXISTS?UMI_STATUS_OK:s;

}

static UmiStatus ReportRead(UmiSetupBundle *b,const char *id,const char *title,const char *path,int application,UmiSetupReport *r)
{

    char *data=NULL;
    size_t size=0;
    UmiStatus s=ScRead(path,UMI_SETUP_MANIFEST_LIMIT,&data,&size,r);
    if(s!=UMI_STATUS_OK)return s;

    if(memchr(data,0,size)) {
        free(data);
        return UMI_STATUS_PARSE_ERROR;
    }
    char *cursor=data,*executable=NULL,*hash=NULL,*target=NULL,*product=NULL,*files=NULL;

    const char *keys[]= {
        "UMI_REPORT_EXECUTABLE","UMI_REPORT_EXECUTABLE_SHA256","UMI_REPORT_TARGET","UMI_REPORT_PRODUCT","UMI_REPORT_FILES"
    };
    char **values[]= {
        &executable,&hash,&target,&product,&files
    };

    for(size_t i=0; i<5U&&s==UMI_STATUS_OK; ++i)s=Assignment(&cursor,keys[i],values[i]);

    if(s==UMI_STATUS_OK&&(*cursor||strcmp(id,target)||strcmp(title,product)))s=UMI_STATUS_PARSE_ERROR;

    unsigned owner=SC_SHARED;

    if(s==UMI_STATUS_OK) {
        const char *name=strrchr(executable,'/'),*back=strrchr(executable,'\\');
        if(back&&(!name||back>name))name=back;
        name=name?name+1:executable;

        char entry[UMI_SETUP_RELATIVE_CAPACITY];
        int n=snprintf(entry,sizeof entry,"bin/%s",name);
        if(n<0||(size_t)n>=sizeof entry)s=UMI_STATUS_CAPACITY_EXCEEDED;

        if(s==UMI_STATUS_OK&&application) {
            if(b->appCount==UMI_SETUP_MAX_APPS)s=UMI_STATUS_CAPACITY_EXCEEDED;
            else {
                for(size_t i=0; i<b->appCount; ++i)if(!strcmp(id,b->apps[i].id))s=UMI_STATUS_PARSE_ERROR;

                if(s==UMI_STATUS_OK) {
                    owner=(unsigned)b->appCount;
                    UmiSetupApplication *a=&b->apps[b->appCount++];
                    strcpy(a->id,id);
                    strcpy(a->title,title);
                    strcpy(a->entry,entry);
                }
            }
        }

        if(s==UMI_STATUS_OK)s=AddSource(b,entry,executable,hash,owner,r);

    }

    if(s==UMI_STATUS_OK&&*files) {
        char *list=files,*item;
        while((item=Next(&list,';'))!=NULL&&s==UMI_STATUS_OK) {
            char *record=item,*rel=Next(&record,'|'),*source=Next(&record,'|'),*expected=Next(&record,'|');
            if(!rel||!source||!expected||record)s=UMI_STATUS_PARSE_ERROR;
            else s=AddSource(b,rel,source,expected,SC_SHARED,r);
        }
    }

    free(data);
    return s;

}

UmiStatus ScImportReports(const char *listFile,UmiSetupBundle *b,UmiSetupReport *r)
{

    char *data=NULL;
    size_t size=0;
    UmiStatus s=ScRead(listFile,1024U*1024U,&data,&size,r);
    if(s!=UMI_STATUS_OK)return s;

    const char *header="UMICOM_SETUP_INPUT\t1\n";
    size_t h=strlen(header);
    if(size<h||memcmp(data,header,h)||memchr(data,0,size)||data[size-1U]!='\n') {
        free(data);
        return UMI_STATUS_PARSE_ERROR;
    }

    char *cursor=data+h,*line;

    while(s==UMI_STATUS_OK&&(line=Next(&cursor,'\n'))!=NULL) {

        if(!*line) {
            if(cursor!=NULL)s=UMI_STATUS_PARSE_ERROR;
            break;
        }

        char *p=line,*kind=Next(&p,'\t');

        if(!strcmp(kind,"app")||!strcmp(kind,"shared")) {
            char *id=Next(&p,'\t'),*title=Next(&p,'\t'),*report=Next(&p,'\t');
            if(!id||!title||!report||p||!ScToken(id)||!strcmp(id,"shared")||!ScUtf8(title)||!*title||strlen(title)>=192U)s=UMI_STATUS_PARSE_ERROR;
            else s=ReportRead(b,id,title,report,!strcmp(kind,"app"),r);
        }

        else if(!strcmp(kind,"data")) {
            char *rel=Next(&p,'\t'),*source=Next(&p,'\t');
            if(!rel||!source||p)s=UMI_STATUS_PARSE_ERROR;
            else s=AddSource(b,rel,source,NULL,SC_SHARED,r);
        }

        else s=UMI_STATUS_PARSE_ERROR;

    }

    if(s==UMI_STATUS_OK&&(!b->appCount||!b->fileCount))s=UMI_STATUS_PARSE_ERROR;
    free(data);
    return s;

}

UmiStatus UmiSetupPack(const char *listFile,const char *destination,const char *bootstrap,UmiSetupProgress progress,void *context,UmiSetupReport *r)
{

    UmiSetupReport local= {
        0
    };
    if(!r)r=&local;
    memset(r,0,sizeof *r);

    if(UmiSetupValidateAbsolute(listFile)!=UMI_STATUS_OK||UmiSetupValidateAbsolute(destination)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;

    UmiSetupBundle *b=calloc(1,sizeof *b);
    if(!b)return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus s=ScImportReports(listFile,b,r);

    if(s==UMI_STATUS_OK&&bootstrap)s=AddSource(b,"bin/umicom-setup-centre.exe",bootstrap,NULL,SC_SHARED,r);

    uint64_t total=0;
    for(size_t i=0; i<b->fileCount&&s==UMI_STATUS_OK; ++i) {
        if(b->files[i].bytes>UMI_SETUP_MAX_TOTAL_BYTES-total)s=UMI_STATUS_CAPACITY_EXCEEDED;
        else total+=b->files[i].bytes;
    }
    r->bytesPlanned=total;

    if(s==UMI_STATUS_OK&&progress&&progress(r,context))s=UMI_STATUS_CANCELLED;

    if(s==UMI_STATUS_OK)s=ScMakeDirectory(destination,r);
    if(s==UMI_STATUS_OK)r->outputCreated=1;

    char payload[UMI_SETUP_PATH_CAPACITY];
    if(s==UMI_STATUS_OK)s=ScJoin(destination,"payload",payload);
    if(s==UMI_STATUS_OK)s=ScMakeDirectory(payload,r);

    for(size_t i=0; i<b->fileCount&&s==UMI_STATUS_OK; ++i) {
        const ScFile *f=&b->files[i];
        char target[UMI_SETUP_PATH_CAPACITY];
        s=ScParents(payload,f->relative,r);
        if(s==UMI_STATUS_OK)s=ScJoin(payload,f->relative,target);

        ScReport(r,s,"Packaging %s",f->relative);
        if(s==UMI_STATUS_OK)s=ScCopy(f->source,target,f->hash,f->bytes,progress,context,r);
        if(s==UMI_STATUS_OK)++r->filesCompleted;
    }

    if(s==UMI_STATUS_OK&&bootstrap) {

        const ScFile *record=NULL;

        for(size_t i=0; i<b->fileCount; ++i)if(!strcmp(b->files[i].relative,"bin/umicom-setup-centre.exe"))record=&b->files[i];

        char target[UMI_SETUP_PATH_CAPACITY];

        if(!record)s=UMI_STATUS_INTERNAL_ERROR;

        if(s==UMI_STATUS_OK)s=ScJoin(destination,"Umicom-Setup.exe",target);

        if(s==UMI_STATUS_OK)s=ScCopy(bootstrap,target,record->hash,record->bytes,progress,context,r);

    }

    if(s==UMI_STATUS_OK) {
        ScText t;
        ScTextInit(&t);
        s=ScEncode(b,UmiSetupAllApplications(b),0,&t);
        char target[UMI_SETUP_PATH_CAPACITY];
        if(s==UMI_STATUS_OK)s=ScJoin(destination,UMI_SETUP_CATALOGUE,target);
        if(s==UMI_STATUS_OK)s=ScWriteNew(target,t.data,t.size,r);
        ScTextFree(&t);
    }

    UmiSetupBundleDestroy(b);
    r->completed=s==UMI_STATUS_OK;
    ScReport(r,s,s==UMI_STATUS_OK?"Offline suite prepared from verified runtime reports. No applications were executed.":"Package not completed. Existing source files were not changed.");
    return s;

}
