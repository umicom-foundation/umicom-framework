/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_inspection.c
 *
 * PURPOSE:
 *   Verify bounded file composition, owned report lifetime and metadata-only JSON refusal semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_inspection.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include "umicom/platform/native_arguments.h"
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%d: %s\n",__LINE__,#x); return 1; } } while (0)
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"inspection-fixture");strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"Inspection fixture");strcpy(c.endpoint,"http://127.0.0.1:8188");
    c.enabled=true;c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;c.timeout_ms=30000;return c;
}
static UmiStatus Report(const char *workflow,const char *nodes,uint64_t revision,UmiComfyPreflightReport **out)
{
    UmiProviderConnection c=Connection();UmiComfyPromptDraft draft={0};
    draft.workflow_json=workflow;draft.workflow_length=strlen(workflow);strcpy(draft.client_id,"inspection-test");
    UmiComfyPrompt *prompt=NULL;UmiComfyNodeCatalogue *catalogue=NULL;
    UmiStatus status=UmiComfyPromptCreate(&c,revision,&draft,NULL,&prompt);
    if(status==UMI_STATUS_OK)status=UmiComfyNodeCatalogueCreate(&c,revision,nodes,strlen(nodes),NULL,&catalogue);
    if(status==UMI_STATUS_OK)status=UmiComfyPreflightCreate(&c,revision,prompt,catalogue,NULL,out);
    UmiComfyNodeCatalogueDestroy(catalogue);UmiComfyPromptDestroy(prompt);return status;
}
static int Text(const UmiJsonTree *tree,int object,const char *key,const char *expected)
{
    int value=-1;char text[256];
    CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeText(tree,value,text,sizeof text)==UMI_STATUS_OK);
    CHECK(!strcmp(text,expected));return 0;
}
static int JsonTests(const char *mode)
{
    const char *workflow="{\"node\\\"\\n\":{\"class_type\":\"Text\\\"\\n\",\"inputs\":{\"private\":\"PRIVATE_SENTINEL_DO_NOT_EMIT\"}}}";
    const char *nodes="{\"Text\\\"\\n\":{\"input\":{\"required\":{\"text\\\"\\n\":[\"STRING\"]}},\"output\":[]}}";
    char *many=NULL;
    if(!strcmp(mode,"json-truncation")){
        many=malloc(32768U);CHECK(many!=NULL);size_t used=1U;many[0]='{';
        for(size_t i=0U;i<300U;++i){int n=snprintf(many+used,32768U-used,"%s\"n%zu\":{\"class_type\":\"Missing\",\"inputs\":{}}",i?",":"",i);CHECK(n>0 && (size_t)n<32768U-used);used+=(size_t)n;}
        strcpy(many+used,"}");workflow=many;nodes="{}";
    }
    UmiComfyPreflightReport *report=NULL;
    CHECK(Report(workflow,nodes,UINT64_MAX,&report)==UMI_STATUS_OK);
    free(many);
    char *json=malloc(UMI_COMFY_INSPECTION_JSON_CAPACITY);CHECK(json!=NULL);size_t length=0U;
    CHECK(UmiComfyInspectionJson(report,json,UMI_COMFY_INSPECTION_JSON_CAPACITY,&length)==UMI_STATUS_OK);
    CHECK(length==strlen(json));
    UmiJsonTree *tree=NULL;CHECK(UmiJsonTreeCreate(json,length,NULL,NULL,&tree)==UMI_STATUS_OK);
    if(!strcmp(mode,"json-escaping")){
        int issues=-1;CHECK(UmiJsonTreeMember(tree,0,"issues",&issues)==UMI_STATUS_OK);
        CHECK(UmiJsonTreeCount(tree,issues)==1U);int item=UmiJsonTreeFirst(tree,issues);
        CHECK(Text(tree,item,"kind","input-missing")==0);
        CHECK(Text(tree,item,"node_id","node\"\n")==0);
        CHECK(Text(tree,item,"class_type","Text\"\n")==0);
        CHECK(Text(tree,item,"input_name","text\"\n")==0);
    }else if(!strcmp(mode,"json-exact")){
        char *exact=malloc(length+1U);CHECK(exact!=NULL);size_t count=99U;
        CHECK(UmiComfyInspectionJson(report,exact,length+1U,&count)==UMI_STATUS_OK);
        CHECK(count==length && !memcmp(exact,json,length+1U));free(exact);
    }else if(!strcmp(mode,"json-refusal")){
        char *out=malloc(length+1U);CHECK(out!=NULL);memset(out,'Q',length+1U);
        size_t count=99U;CHECK(UmiComfyInspectionJson(report,out,length,&count)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(count==99U);for(size_t i=0U;i<=length;++i)CHECK(out[i]=='Q');
        CHECK(UmiComfyInspectionJson(report,out,0U,&count)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(out[0]=='Q' && count==99U);free(out);
    }else if(!strcmp(mode,"json-max-revision")){
        CHECK(Text(tree,0,"connection_revision","18446744073709551615")==0);
    }else if(!strcmp(mode,"json-digests")){
        UmiComfyPreflightSummary s;char digest[65];CHECK(UmiComfyPreflightInspect(report,&s)==UMI_STATUS_OK);
        UmiSha256Hex(s.connection_sha256,digest);CHECK(Text(tree,0,"connection_sha256",digest)==0);
        UmiSha256Hex(s.request_sha256,digest);CHECK(Text(tree,0,"request_sha256",digest)==0);
        UmiSha256Hex(s.catalogue_sha256,digest);CHECK(Text(tree,0,"catalogue_sha256",digest)==0);
    }else if(!strcmp(mode,"json-private-values")){
        CHECK(strstr(json,"PRIVATE_SENTINEL_DO_NOT_EMIT")==NULL);
        int absent=-1;CHECK(UmiJsonTreeMember(tree,0,"workflow",&absent)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiJsonTreeMember(tree,0,"origin",&absent)==UMI_STATUS_NOT_FOUND);
    }else if(!strcmp(mode,"json-truncation")){
        int value=-1;int64_t count=0;
        CHECK(UmiJsonTreeMember(tree,0,"issue_count",&value)==UMI_STATUS_OK);
        CHECK(UmiJsonTreeInteger(tree,value,&count)==UMI_STATUS_OK && count==300);
        CHECK(UmiJsonTreeMember(tree,0,"omitted_issue_count",&value)==UMI_STATUS_OK);
        CHECK(UmiJsonTreeInteger(tree,value,&count)==UMI_STATUS_OK && count==44);
        CHECK(UmiJsonTreeMember(tree,0,"issues",&value)==UMI_STATUS_OK && UmiJsonTreeCount(tree,value)==256U);
    }else if(!strcmp(mode,"json-null")){
        char out[8]="before";size_t count=13U;
        CHECK(UmiComfyInspectionJson(NULL,out,sizeof out,&count)!=UMI_STATUS_OK);
        CHECK(!strcmp(out,"before") && count==13U);
        CHECK(UmiComfyInspectionJson(report,NULL,sizeof out,&count)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyInspectionJson(report,out,sizeof out,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!strcmp(out,"before") && count==13U);
    }else CHECK(0);
    UmiJsonTreeDestroy(tree);UmiComfyPreflightDestroy(report);free(json);return 0;
}
/* Use native UTF-8 argument decoding so the Unicode-path case tests the same contract as production Windows clients. Retain the earlier entry signature for review. The former implementation is retained for owner review. */
#if 0
int main(int argc,char **argv)
#endif

/* Exercise the same file admission through the inventory wrapper too. This
 * prevents later reference-specific work from weakening the shared boundary. */
static int ReferenceFileCheck(const UmiComfyInspectionPaths *paths,
    const UmiCancellationToken *cancel,const char *mode)
{
    UmiComfyReferenceInventory *inventory=NULL;
    if(!strcmp(mode,"null-output")){
        CHECK(UmiComfyReferenceInspectionRead(paths,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;
    }
/* The file-boundary check is invoked by the outer test runner after input mutation. Retain this misplaced recursive call for review; it must not execute inside the helper. The former implementation is retained for owner review. */
#if 0
    CHECK(ReferenceFileCheck(&paths,cancel,mode)==0);
#endif

    if(!strcmp(mode,"occupied")){
        CHECK(UmiComfyReferenceInspectionRead(paths,NULL,&inventory)==UMI_STATUS_OK);
        UmiComfyReferenceInventory *prior=inventory;
        CHECK(UmiComfyReferenceInspectionRead(paths,NULL,&inventory)==UMI_STATUS_INVALID_ARGUMENT && inventory==prior);
        UmiComfyReferenceInventoryDestroy(inventory);return 0;
    }
    UmiStatus status=UmiComfyReferenceInspectionRead(paths,cancel,&inventory);
    if(!strcmp(mode,"complete") || !strcmp(mode,"unicode-path") || !strcmp(mode,"missing") || !strcmp(mode,"missing-class")){
        CHECK(status==UMI_STATUS_OK && inventory!=NULL);UmiComfyReferenceSummary summary;
        CHECK(UmiComfyReferenceInventoryInspect(inventory,&summary)==UMI_STATUS_OK);
        CHECK(summary.connection_revision==7U && summary.node_count==1U && summary.candidate_count==0U);
        char json[4096];size_t length=0U;
        CHECK(UmiComfyReferenceInspectionJson(inventory,0U,1U,json,sizeof json,&length)==UMI_STATUS_OK);
        CHECK(strstr(json,"PRIVATE_SENTINEL_DO_NOT_EMIT")==NULL);UmiComfyReferenceInventoryDestroy(inventory);
    }else{
        CHECK(status!=UMI_STATUS_OK && inventory==NULL);
        if(!strcmp(mode,"cancel"))CHECK(status==UMI_STATUS_CANCELLED);
        if(strstr(mode,"limit"))CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
        if(!strcmp(mode,"disabled"))CHECK(status==UMI_STATUS_PERMISSION_DENIED);
        if(!strncmp(mode,"client-",7U)||!strcmp(mode,"null-path")||!strcmp(mode,"relative-before-read"))CHECK(status==UMI_STATUS_INVALID_ARGUMENT);
    }
    return 0;
}

static int InspectionTest(int argc,char **argv)
{
    CHECK(argc==5);const char *mode=argv[1];
    if(!strncmp(mode,"json-",5U))return JsonTests(mode);
    UmiComfyInspectionPaths paths={0};paths.connection_document=argv[2];paths.workflow_json=argv[3];paths.node_catalogue_json=argv[4];
    strcpy(paths.client_id,"inspection-test");
    UmiComfyPreflightReport *report=NULL;UmiCancellationToken *cancel=NULL;
    if(!strcmp(mode,"relative-before-read")){/* Retain the valid absolute first path so only the third path is malformed on Windows as well as POSIX. The earlier platform-specific absolute spelling is preserved for review. The former implementation is retained for owner review. */
#if 0
paths.connection_document="/definitely-absent-umicom-file";paths.node_catalogue_json="relative.json";
#endif
paths.node_catalogue_json="relative.json";}
    if(!strcmp(mode,"null-path"))paths.node_catalogue_json=NULL;
    if(!strcmp(mode,"client-empty"))paths.client_id[0]='\0';
    if(!strcmp(mode,"client-unterminated"))memset(paths.client_id,'x',sizeof paths.client_id);
    if(!strcmp(mode,"client-utf8")){paths.client_id[0]=(char)0xc0;paths.client_id[1]=(char)0xaf;paths.client_id[2]='\0';}
    if(!strcmp(mode,"cancel")){CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);}
    CHECK(ReferenceFileCheck(&paths,cancel,mode)==0);
    if(!strcmp(mode,"occupied")){
        CHECK(Report("{\"1\":{\"class_type\":\"N\",\"inputs\":{}}}","{}",1U,&report)==UMI_STATUS_OK);
        UmiComfyPreflightReport *before=report;CHECK(UmiComfyInspectionRead(&paths,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report==before);UmiComfyPreflightDestroy(report);return 0;
    }
    if(!strcmp(mode,"null-output")){CHECK(UmiComfyInspectionRead(&paths,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);return 0;}
    UmiStatus status=UmiComfyInspectionRead(&paths,cancel,&report);
    umi_cancellation_token_destroy(cancel);
    if(!strcmp(mode,"complete") || !strcmp(mode,"unicode-path") || !strcmp(mode,"missing") || !strcmp(mode,"missing-class")){
        CHECK(status==UMI_STATUS_OK && report!=NULL);UmiComfyPreflightSummary s;
        CHECK(UmiComfyPreflightInspect(report,&s)==UMI_STATUS_OK && s.connection_revision==7U && s.node_count==1U);
        if(!strcmp(mode,"missing"))CHECK(!s.all_node_requirements_observed && s.missing_required_count==1U);
        else if(!strcmp(mode,"missing-class"))CHECK(!s.all_node_requirements_observed && s.unknown_node_count==1U);
        else CHECK(s.all_node_requirements_observed && s.known_node_count==1U);
        char json[4096];size_t length=0U;CHECK(UmiComfyInspectionJson(report,json,sizeof json,&length)==UMI_STATUS_OK);
        CHECK(strstr(json,"PRIVATE_SENTINEL_DO_NOT_EMIT")==NULL);UmiComfyPreflightDestroy(report);
    }else{
        CHECK(status!=UMI_STATUS_OK && report==NULL);
        if(!strcmp(mode,"cancel"))CHECK(status==UMI_STATUS_CANCELLED);
        if(strstr(mode,"limit")!=NULL)CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
        if(!strcmp(mode,"disabled"))CHECK(status==UMI_STATUS_PERMISSION_DENIED);
        if(!strncmp(mode,"client-",7U) || !strcmp(mode,"null-path") || !strcmp(mode,"relative-before-read"))CHECK(status==UMI_STATUS_INVALID_ARGUMENT);
    }
    return 0;
}

int main(int argc,char **argv)
{
    int result=1;UmiStatus status=UmiNativeArgumentsDispatch(argc,argv,InspectionTest,&result);
    return status==UMI_STATUS_OK ? result : 1;
}
