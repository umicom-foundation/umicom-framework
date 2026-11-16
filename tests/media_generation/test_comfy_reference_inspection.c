/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_reference_inspection.c
 *
 * PURPOSE:
 *   Verify complete reference counts, bounded pages, lossless values and atomic JSON output.
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
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}}while(0)
static int Text(const UmiJsonTree *tree,int object,const char *key,const char *expected)
{
    int value=-1;char text[256];CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeText(tree,value,text,sizeof text)==UMI_STATUS_OK);CHECK(!strcmp(text,expected));return 0;
}
static int Number(const UmiJsonTree *tree,int object,const char *key,int64_t expected)
{
    int value=-1;int64_t n=-1;CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeInteger(tree,value,&n)==UMI_STATUS_OK && n==expected);return 0;
}
static int Null(const UmiJsonTree *tree,int object,const char *key)
{
/* Match the shared source-span API output type exactly; pointer-to-pointer parameters cannot use the generic void-pointer conversion. The former implementation is retained for owner review. */
#if 0
    int value=-1;const void *bytes=NULL;size_t length=0U;
#endif
    int value=-1;const char *bytes=NULL;size_t length=0U;
    CHECK(UmiJsonTreeMember(tree,object,key,&value)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeSourceSpan(tree,value,&bytes,&length)==UMI_STATUS_OK);
    CHECK(length==4U && !memcmp(bytes,"null",4U));return 0;
}
static int Refusal(const UmiComfyReferenceInventory *inventory,size_t offset,size_t limit,
    size_t capacity,UmiStatus expected)
{
    char output[64];memset(output,'Q',sizeof output);size_t length=77U;
    CHECK(UmiComfyReferenceInspectionJson(inventory,offset,limit,output,capacity,&length)==expected);
    CHECK(length==77U);for(size_t i=0U;i<sizeof output;++i)CHECK(output[i]=='Q');return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    const char *workflow="{\"root\":{\"class_type\":\"N\",\"inputs\":{}},\"unknown\":{\"class_type\":\"Absent\",\"inputs\":{}},\"reader\":{\"class_type\":\"N\",\"inputs\":{\"a\\\"\\n\":[\"root\",0],\"b\":[\"missing\\\"\\n\",9223372036854775807],\"c\":[\"root\",true],\"d\":[\"unknown\",0],\"private\":\"PRIVATE_SENTINEL_DO_NOT_EMIT\",\"ordinary\":[1,2]}}}";
    const char *nodes="{\"N\":{\"input\":{},\"output\":[\"T\\\"\\n\"]}}";
    char *many=NULL;
    if(!strcmp(mode,"empty"))workflow="{\"root\":{\"class_type\":\"N\",\"inputs\":{}}}";
    if(!strcmp(mode,"maximum-page")){
        many=malloc(32768U);CHECK(many!=NULL);
        size_t used=(size_t)snprintf(many,32768U,"{\"root\":{\"class_type\":\"N\",\"inputs\":{");
        for(size_t i=0U;i<300U;++i){int n=snprintf(many+used,32768U-used,"%s\"i%zu\":[\"root\",0]",i?",":"",i);CHECK(n>0 && (size_t)n<32768U-used);used+=(size_t)n;}
        strcpy(many+used,"}}}");workflow=many;
    }
    UmiProviderConnection connection={0};strcpy(connection.id,"reference-json-fixture");
    strcpy(connection.provider_id,"comfyui");strcpy(connection.label,"Reference fixture");
    strcpy(connection.endpoint,"http://127.0.0.1:8188");connection.enabled=true;
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;
    UmiComfyPromptDraft draft={0};draft.workflow_json=workflow;draft.workflow_length=strlen(workflow);
    strcpy(draft.client_id,"reference-json-test");
    UmiComfyPrompt *prompt=NULL;UmiComfyNodeCatalogue *catalogue=NULL;UmiComfyReferenceInventory *inventory=NULL;
    CHECK(UmiComfyPromptCreate(&connection,UINT64_MAX,&draft,NULL,&prompt)==UMI_STATUS_OK);
    CHECK(UmiComfyNodeCatalogueCreate(&connection,UINT64_MAX,nodes,strlen(nodes),NULL,&catalogue)==UMI_STATUS_OK);
    CHECK(UmiComfyReferenceInventoryCreate(&connection,UINT64_MAX,prompt,catalogue,NULL,&inventory)==UMI_STATUS_OK);
    UmiComfyPromptDestroy(prompt);UmiComfyNodeCatalogueDestroy(catalogue);free(many);
    char *json=malloc(UMI_COMFY_REFERENCE_JSON_CAPACITY);CHECK(json!=NULL);size_t length=0U;
    size_t offset=!strcmp(mode,"second-page")?1U:!strcmp(mode,"last-page")?3U:!strcmp(mode,"end-page")?4U:0U;
    size_t limit=!strcmp(mode,"first-page")||!strcmp(mode,"second-page")?1U:256U;
    CHECK(UmiComfyReferenceInspectionJson(inventory,offset,limit,json,UMI_COMFY_REFERENCE_JSON_CAPACITY,&length)==UMI_STATUS_OK);
    CHECK(length==strlen(json));UmiJsonTree *tree=NULL;
    CHECK(UmiJsonTreeCreate(json,length,NULL,NULL,&tree)==UMI_STATUS_OK);
    int array=-1;CHECK(UmiJsonTreeMember(tree,0,"candidates",&array)==UMI_STATUS_OK);
    int item=UmiJsonTreeFirst(tree,array);
    if(!strcmp(mode,"summary")){
        CHECK(!Text(tree,0,"scope","reference-candidate-observations"));
        CHECK(!Text(tree,0,"connection_revision","18446744073709551615"));
        CHECK(!Number(tree,0,"node_count",3)&&!Number(tree,0,"candidate_count",4));
        CHECK(!Number(tree,0,"other_array_count",1)&&!Number(tree,0,"unsupported_slot_count",1));
        CHECK(!Number(tree,0,"missing_target_count",1)&&!Number(tree,0,"unobserved_output_count",1));
        CHECK(!Number(tree,0,"observed_output_count",1)&&!Number(tree,0,"out_of_range_count",0));
    }else if(!strcmp(mode,"first-page")||!strcmp(mode,"second-page")||!strcmp(mode,"last-page")){
        CHECK(UmiJsonTreeCount(tree,array)==1U);CHECK(!Number(tree,0,"offset",(int64_t)offset));
        CHECK(!Number(tree,0,"remaining_count",(int64_t)(3U-offset)));
        CHECK(!Number(tree,0,"next_offset",(int64_t)(offset+1U)));
        CHECK(!Text(tree,item,"input_name",offset==0U?"a\"\n":offset==1U?"b":"d"));
    }else if(!strcmp(mode,"end-page")||!strcmp(mode,"empty")){
        CHECK(UmiJsonTreeCount(tree,array)==0U && !Number(tree,0,"page_count",0));
        CHECK(!Number(tree,0,"remaining_count",0));
    }else if(!strcmp(mode,"beyond-page"))CHECK(!Refusal(inventory,5U,1U,64U,UMI_STATUS_NOT_FOUND));
    else if(!strcmp(mode,"zero-limit"))CHECK(!Refusal(inventory,0U,0U,64U,UMI_STATUS_INVALID_ARGUMENT));
    else if(!strcmp(mode,"large-limit"))CHECK(!Refusal(inventory,0U,257U,64U,UMI_STATUS_INVALID_ARGUMENT));
    else if(!strcmp(mode,"zero-capacity"))CHECK(!Refusal(inventory,0U,1U,0U,UMI_STATUS_CAPACITY_EXCEEDED));
    else if(!strcmp(mode,"exact-capacity")||!strcmp(mode,"short-capacity")){
        char *out=malloc(length+1U);CHECK(out!=NULL);memset(out,'Q',length+1U);size_t n=77U;
        if(!strcmp(mode,"exact-capacity")){
            CHECK(UmiComfyReferenceInspectionJson(inventory,0U,256U,out,length+1U,&n)==UMI_STATUS_OK);
            CHECK(n==length && !memcmp(out,json,length+1U));
        }else{
            CHECK(UmiComfyReferenceInspectionJson(inventory,0U,256U,out,length,&n)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(n==77U);for(size_t i=0U;i<=length;++i)CHECK(out[i]=='Q');
        }free(out);
    }else if(!strcmp(mode,"nulls")){
        CHECK(!Refusal(NULL,0U,1U,64U,UMI_STATUS_INVALID_ARGUMENT));size_t n=77U;char out[64];
        CHECK(UmiComfyReferenceInspectionJson(inventory,0U,1U,NULL,64U,&n)==UMI_STATUS_INVALID_ARGUMENT && n==77U);
        CHECK(UmiComfyReferenceInspectionJson(inventory,0U,1U,out,64U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(!strcmp(mode,"escaping")){
        CHECK(!Text(tree,item,"input_name","a\"\n")&&!Text(tree,item,"output_type","T\"\n"));
        CHECK(!Text(tree,UmiJsonTreeNext(tree,item),"target_id","missing\"\n"));
    }else if(!strcmp(mode,"max-integer"))CHECK(!Text(tree,UmiJsonTreeNext(tree,item),"output_slot","9223372036854775807"));
    else if(!strcmp(mode,"unknown-slot"))CHECK(!Null(tree,UmiJsonTreeNext(tree,UmiJsonTreeNext(tree,item)),"output_slot"));
    else if(!strcmp(mode,"unknown-output")){
        int last=UmiJsonTreeNext(tree,UmiJsonTreeNext(tree,UmiJsonTreeNext(tree,item)));
        CHECK(!Null(tree,last,"declared_output_count"));CHECK(!Text(tree,last,"output_type",""));
    }else if(!strcmp(mode,"private-values")){
        CHECK(strstr(json,"PRIVATE_SENTINEL_DO_NOT_EMIT")==NULL);CHECK(strstr(json,"missing\\\"\\n")!=NULL);
    }else if(!strcmp(mode,"digests")){
        UmiComfyReferenceSummary summary;char digest[65];CHECK(UmiComfyReferenceInventoryInspect(inventory,&summary)==UMI_STATUS_OK);
        UmiSha256Hex(summary.connection_sha256,digest);CHECK(!Text(tree,0,"connection_sha256",digest));
        UmiSha256Hex(summary.request_sha256,digest);CHECK(!Text(tree,0,"request_sha256",digest));
        UmiSha256Hex(summary.catalogue_sha256,digest);CHECK(!Text(tree,0,"catalogue_sha256",digest));
    }else if(!strcmp(mode,"repeat")){
        char *again=malloc(length+1U);CHECK(again!=NULL);size_t n=0U;
        CHECK(UmiComfyReferenceInspectionJson(inventory,0U,256U,again,length+1U,&n)==UMI_STATUS_OK);
        CHECK(n==length&&!memcmp(again,json,length+1U));free(again);
    }else if(!strcmp(mode,"all-pages")){
        for(size_t i=0U;i<4U;++i){size_t n=0U;CHECK(UmiComfyReferenceInspectionJson(inventory,i,1U,json,UMI_COMFY_REFERENCE_JSON_CAPACITY,&n)==UMI_STATUS_OK);
            UmiJsonTree *page=NULL;CHECK(UmiJsonTreeCreate(json,n,NULL,NULL,&page)==UMI_STATUS_OK);
            CHECK(!Number(page,0,"offset",(int64_t)i)&&!Number(page,0,"page_count",1));UmiJsonTreeDestroy(page);}
    }else if(!strcmp(mode,"maximum-page")){
        CHECK(UmiJsonTreeCount(tree,array)==256U&&!Number(tree,0,"remaining_count",44));
        CHECK(UmiComfyReferenceInspectionJson(inventory,256U,256U,json,UMI_COMFY_REFERENCE_JSON_CAPACITY,&length)==UMI_STATUS_OK);
        UmiJsonTree *page=NULL;CHECK(UmiJsonTreeCreate(json,length,NULL,NULL,&page)==UMI_STATUS_OK);
        CHECK(!Number(page,0,"page_count",44)&&!Number(page,0,"remaining_count",0));UmiJsonTreeDestroy(page);
    }else CHECK(0);
    UmiJsonTreeDestroy(tree);UmiComfyReferenceInventoryDestroy(inventory);free(json);return 0;
}
