/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * File: src/release_inspector/cli.c
 * Purpose: Native release diagnostics, with exclusive JSON report publication.
 * An output report describes this observation; it is not a release signature,
 * installer receipt or evidence that Windows actually started the application.
 *---------------------------------------------------------------------------*/
#include "umicom/release_inspector/inspection.h"
#include "../setup_centre/internal.h"
#include <inttypes.h>
typedef struct Output {
    ScText text;
    int first;
    int json;
} Output;
static void Json(ScText *text,const char *value)
{
    ScPrint(text,"\"");
    for(const unsigned char *p=(const unsigned char *)value;*p;++p) {
        if(*p=='"'||*p=='\\')ScPrint(text,"\\%c",*p);
        else if(*p<32U || *p==127U)ScPrint(text,"\\u%04x",(unsigned)*p);
        else ScPrint(text,"%c",*p);
    }
    ScPrint(text,"\"");
}
static const char *Kind(UmiReleaseObservationKind kind)
{
    switch(kind) {
        case UMI_RELEASE_FILE_CHECKED:return "file";
        case UMI_RELEASE_PRIVATE_IMPORT:return "private-import";
        case UMI_RELEASE_SYSTEM_IMPORT:return "system-deferred";
        case UMI_RELEASE_RESOURCE_CHECKED:return "resource";
        case UMI_RELEASE_ISSUE:return "issue";
        default:return "unknown";
    }
}
static int Observe(const UmiReleaseObservation *item,void *opaque)
{
    Output *o=opaque;
    if(!item->path[0])return 0; /* Cancellation heartbeat, not a file result. */
    if(item->kind==UMI_RELEASE_ISSUE)
        fprintf(stderr,"%s: %s%s%s\n",item->path,item->detail,
            item->dependency[0]?" Dependency: ":"",item->dependency);
    if(!o->json)return 0;
    ScPrint(&o->text,"%s{\"kind\":",o->first?"":",\n");o->first=0;
    Json(&o->text,Kind(item->kind));
    ScPrint(&o->text,",\"status\":%d,\"path\":",(int)item->status);Json(&o->text,item->path);
    ScPrint(&o->text,",\"dependency\":");Json(&o->text,item->dependency);
    ScPrint(&o->text,",\"delayed\":%s,\"detail\":",item->delayed?"true":"false");
    Json(&o->text,item->detail);ScPrint(&o->text,"}");
    return o->text.status!=UMI_STATUS_OK;
}
static uint64_t Select(const UmiSetupBundle *b,const char *list)
{
    if(!list || !strcmp(list,"all"))return UmiSetupAllApplications(b);
    if(!*list || strlen(list)>8191U)return 0U;
    char copy[8192];strcpy(copy,list);
    uint64_t mask=0U;char *at=copy;
    while(at) {
        char *next=strchr(at,',');if(next)*next++=0;
        size_t i;
        for(i=0U;i<UmiSetupApplicationCount(b);++i)
            if(!strcmp(at,UmiSetupApplicationAt(b,i)->id))break;
        if(i==UmiSetupApplicationCount(b) || ((mask>>i)&UINT64_C(1)))return 0U;
        mask|=UINT64_C(1)<<i;at=next;
    }
    return mask;
}
static void Help(void)
{
    puts("Umicom release inspector (native C23)\n"
        "  release --root ABSOLUTE [--apps all|id,id] [--output NEW_JSON]\n"
        "  installation --root ABSOLUTE [--output NEW_JSON]\n"
        "  pe --file ABSOLUTE\n"
        "  --self-test\n"
        "Reads recorded files; never loads a DLL, starts an application or searches PATH.\n"
        "System DLLs are deferred host requirements. Static success is not a startup test.\n"
        "JSON output is optional, bounded and never overwrites an existing file.");
}
int UmiReleaseInspectorMain(int argc,char **argv)
{
    if(argc<1 || !argv)return 2;
    for(int i=0;i<argc;++i)if(!argv[i])return 2;
    if(argc==2 && !strcmp(argv[1],"--help")){Help();return 0;}
    if(argc==2 && !strcmp(argv[1],"--self-test")) {
        UmiReleasePeInfo *info=malloc(sizeof *info);if(!info)return 1;
        unsigned char invalid[64]={0};
        int ok=UmiReleasePeInspect(invalid,sizeof invalid,info)==UMI_STATUS_PARSE_ERROR &&
            UmiSetupValidateRelative("bin/notes.txt:stream")!=UMI_STATUS_OK;
        free(info);
        if(ok)puts("Native release checks passed. No file, application or network was opened.");
        return ok?0:1;
    }
    if(argc<4 || (argc%2)!=0){Help();return 2;}
    const char *root=NULL,*file=NULL,*apps=NULL,*output=NULL;
    for(int i=2;i<argc;i+=2) {
        const char **value=NULL;
        if(!strcmp(argv[i],"--root"))value=&root;
        else if(!strcmp(argv[i],"--file"))value=&file;
        else if(!strcmp(argv[i],"--apps"))value=&apps;
        else if(!strcmp(argv[i],"--output"))value=&output;
        if(!value || *value){Help();return 2;}
        *value=argv[i+1];
    }
    if(!strcmp(argv[1],"pe")) {
        if(!file || root || apps || output)return 2;
        UmiSetupReport r={0};char *data=NULL;size_t length=0U;
        UmiReleasePeInfo *info=malloc(sizeof *info);if(!info)return 1;
        UmiStatus status=ScRead(file,UMI_RELEASE_MAX_PE_BYTES,&data,&length,&r);
        if(status==UMI_STATUS_OK)status=UmiReleasePeInspect(data,length,info);
        if(status==UMI_STATUS_OK) {
            printf("PE machine=0x%04x magic=0x%04x subsystem=%u sections=%u imports=%zu\n",
                (unsigned)info->machine,(unsigned)info->optionalMagic,(unsigned)info->subsystem,
                (unsigned)info->sectionCount,info->importCount);
            for(size_t i=0U;i<info->importCount;++i)
                printf("%s\t%s\n",info->delayed[i]?"delay":"normal",info->imports[i]);
            puts("Metadata read only. No Windows loader or application was run.");
        } else fprintf(stderr,"PE inspection failed (%d). %s\n",(int)status,r.detail);
        free(info);free(data);return status==UMI_STATUS_OK?0:1;
    }
    int installed=!strcmp(argv[1],"installation");
    if((!installed && strcmp(argv[1],"release")) || !root || file || (installed && apps) ||
        (output && UmiSetupValidateAbsolute(output)!=UMI_STATUS_OK))return 2;
    UmiSetupBundle *b=NULL;UmiSetupReport io={0};UmiReleaseInspection result={0};
    UmiStatus status=installed?UmiSetupInstalledBundleOpen(root,&b,&io):UmiSetupBundleOpen(root,&b,&io);
    Output o={0};o.first=1;o.json=output!=NULL;ScTextInit(&o.text);
    if(o.json) {
        ScPrint(&o.text,"{\"schema\":\"umicom.release-inspection\",\"root\":");Json(&o.text,root);
        ScPrint(&o.text,",\"input\":\"%s\",\"observations\":[\n",installed?"receipt":"catalogue");
    }
    if(status==UMI_STATUS_OK) {
        uint64_t selected=Select(b,apps);
        status=UmiReleaseInspectBundle(b,selected,Observe,&o,&result);
    } else {result.status=status;(void)snprintf(result.firstIssue,sizeof result.firstIssue,"%.319s",io.detail);}
    if(o.text.status!=UMI_STATUS_OK){status=o.text.status;result.status=status;result.complete=0;}
    char summary[512]={0};
    (void)UmiReleaseInspectionSummary(&result,summary,sizeof summary);
    puts(summary);
    if(result.firstIssue[0])fprintf(stderr,"First issue: %s\n",result.firstIssue);
    if(o.json) {
        ScPrint(&o.text,"\n],\"status\":%d,\"complete\":%s,\"files\":%zu,\"images\":%zu,"
            "\"private_imports\":%zu,\"system_deferred\":%zu,\"delayed_imports\":%zu,"
            "\"resources\":%zu,\"issues\":%zu,\"bytes\":\"%" PRIu64 "\","
            "\"runtime_tested\":false,\"authenticity_tested\":false,\"summary\":",
            (int)status,result.complete?"true":"false",result.filesChecked,result.imagesChecked,
            result.privateImports,result.systemImports,result.delayedImports,
            result.resourcesChecked,result.issues,result.bytesChecked);
        Json(&o.text,summary);ScPrint(&o.text,"}\n");
        UmiStatus written=o.text.status;
        if(written==UMI_STATUS_OK)written=ScWriteNew(output,o.text.data,o.text.size,&io);
        if(written!=UMI_STATUS_OK) {
            fprintf(stderr,"Report not published (%d). Existing files were not replaced.\n",(int)written);
            status=written;
        }
    }
    ScTextFree(&o.text);UmiSetupBundleDestroy(b);
    return status==UMI_STATUS_OK?0:1;
}
