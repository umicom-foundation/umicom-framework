/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_references.c
 *
 * PURPOSE:
 *   Verify copied reference observations, honest unknown fields, bounded inventories and refusal preservation without server execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_references.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;} } while(0)
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"comfy-references");strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"Captured reference fixture");strcpy(c.endpoint,"http://127.0.0.1:8188");
    c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;c.timeout_ms=30000;c.enabled=true;return c;
}
static char *Many(size_t count,int separate_nodes)
{
    size_t capacity=count*100U+256U,used=0U;char *json=malloc(capacity);if(!json)return NULL;
    const char *prefix=separate_nodes?"{":"{\"n\":{\"class_type\":\"N\",\"inputs\":{";
    strcpy(json,prefix);used=strlen(json);
    for(size_t i=0U;i<count;++i){int n;
        if(separate_nodes)n=snprintf(json+used,capacity-used,"%s\"n%zu\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"n%zu\",0]}}",i?",":"",i,i);
        else n=snprintf(json+used,capacity-used,"%s\"input%zu\":[\"n\",0]",i?",":"",i);
        if(n<0 || (size_t)n>=capacity-used){free(json);return NULL;}used+=(size_t)n;
    }
    strcpy(json+used,separate_nodes?"}":"}}}");return json;
}
static const struct Fixture {
    const char *name,*workflow,*schema;
    size_t count,other,unsupported,missing,unknown,out,observed;
} fixtures[]={
    {"empty","{\"n\":{\"class_type\":\"N\",\"inputs\":{\"text\":\"ordinary\"}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",0U,0U,0U,0U,0U,0U,0U},
    {"resolved","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,0U,0U,0U,1U},
    {"missing","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"absent\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,1U,0U,0U,0U},
    {"empty-target","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,1U,0U,0U,0U},
    {"target-case","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"Source\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,1U,0U,0U,0U},
    {"unknown-class","{\"source\":{\"class_type\":\"Unknown\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,0U,1U,0U,0U},
    {"empty-catalogue","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",0]}}}","{}",1U,0U,0U,0U,1U,0U,0U},
    {"zero-outputs","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",0]}}}","{\"N\":{\"input\":{},\"output\":[]}}",1U,0U,0U,0U,0U,1U,0U},
    {"out-of-range","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",1]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,0U,0U,1U,0U},
    {"maximum-slot","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",9223372036854775807]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,0U,0U,1U,0U},
    {"overflow-slot","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",9223372036854775808]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,0U,0U,0U,0U},
    {"negative-slot","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",-1]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,0U,0U,0U,0U},
    {"negative-zero","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",-0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,0U,0U,0U,0U},
    {"float-slot","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",0.0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,0U,0U,0U,0U},
    {"exponent-slot","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",0e0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,0U,0U,0U,0U},
    {"boolean-slot","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",true]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,0U,0U,0U,0U},
    {"null-slot","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",null]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,0U,0U,0U,0U},
    {"text-slot","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",\"0\"]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,0U,0U,0U,0U},
    {"container-slot","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",{}]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,0U,0U,0U,0U},
    {"independent-unknowns","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"absent\",-1]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,1U,1U,0U,0U,0U},
    {"other-arrays","{\"n\":{\"class_type\":\"N\",\"inputs\":{\"a\":[],\"b\":[1,0],\"c\":[\"n\",0,1],\"d\":{\"nested\":[\"n\",0]},\"e\":[[\"n\",0]]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",0U,4U,0U,0U,0U,0U,0U},
    {"self-reference","{\"n\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"n\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,0U,0U,0U,1U},
    {"cycle-observation","{\"a\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"b\",0]}},\"b\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"a\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",2U,0U,0U,0U,0U,0U,2U},
    {"raw-link-metadata","{\"source\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"source\",0]}}}","{\"N\":{\"input\":{\"required\":{\"image\":[\"IMAGE\",{\"rawLink\":true}]}},\"output\":[\"IMAGE\"]}}",1U,0U,0U,0U,0U,0U,1U},
    {"unicode","{\"源\":{\"class_type\":\"N\",\"inputs\":{}},\"接收\":{\"class_type\":\"N\",\"inputs\":{\"图像\":[\"源\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,0U,0U,0U,1U},
    {"escaped-target","{\"so\\u0075rce\":{\"class_type\":\"N\",\"inputs\":{}},\"sink\":{\"class_type\":\"N\",\"inputs\":{\"im\\u0061ge\":[\"sour\\u0063e\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",1U,0U,0U,0U,0U,0U,1U},
    {"source-order","{\"z\":{\"class_type\":\"N\",\"inputs\":{\"last\":[\"a\",0],\"first\":[\"z\",0]}},\"a\":{\"class_type\":\"N\",\"inputs\":{\"middle\":[\"z\",0]}}}","{\"N\":{\"input\":{},\"output\":[\"IMAGE\"]}}",3U,0U,0U,0U,0U,0U,3U}
};
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];size_t fixture=SIZE_MAX;
    const char *workflow=fixtures[1].workflow,*schema=fixtures[1].schema;
    for(size_t i=0U;i<sizeof fixtures/sizeof fixtures[0];++i)if(!strcmp(mode,fixtures[i].name)){
        fixture=i;workflow=fixtures[i].workflow;schema=fixtures[i].schema;break;
    }
    char *owned=NULL;char custom[2048],name[130];
    if(!strcmp(mode,"maximum-candidates")||!strcmp(mode,"too-many-candidates")||!strcmp(mode,"maximum-nodes")){
        owned=Many(!strcmp(mode,"maximum-nodes")?1024U:(!strcmp(mode,"too-many-candidates")?8193U:8192U),!strcmp(mode,"maximum-nodes"));
        CHECK(owned!=NULL);workflow=owned;
    }else if(!strcmp(mode,"maximum-name")||!strcmp(mode,"long-name")||!strcmp(mode,"long-target")){
        memset(name,'a',sizeof name);name[!strcmp(mode,"maximum-name")?128U:129U]=0;
        if(!strcmp(mode,"long-target"))CHECK(snprintf(custom,sizeof custom,"{\"n\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"%s\",0]}}}",name)>0);
        else CHECK(snprintf(custom,sizeof custom,"{\"n\":{\"class_type\":\"N\",\"inputs\":{\"%s\":[\"n\",0]}}}",name)>0);
        workflow=custom;
    }else if(!strcmp(mode,"last-output")){
        size_t used=(size_t)snprintf(custom,sizeof custom,"{\"N\":{\"input\":{},\"output\":[");
        for(size_t i=0U;i<64U;++i)used+=(size_t)snprintf(custom+used,sizeof custom-used,"%s\"T%zu\"",i?",":"",i);
        strcpy(custom+used,"]}}");schema=custom;
        workflow="{\"n\":{\"class_type\":\"N\",\"inputs\":{\"image\":[\"n\",63]}}}";
    }
    UmiProviderConnection c=Connection(),current=c,observed=c;
    if(!strcmp(mode,"catalogue-settings"))strcpy(observed.label,"Changed observation label");
    UmiComfyPromptDraft draft={0};draft.workflow_json=workflow;draft.workflow_length=strlen(workflow);strcpy(draft.client_id,"reference-fixture");
    UmiComfyPrompt *prompt=NULL;UmiComfyNodeCatalogue *catalogue=NULL;UmiComfyReferenceInventory *inventory=NULL;
    CHECK(UmiComfyPromptCreate(&c,7U,&draft,NULL,&prompt)==UMI_STATUS_OK);
    CHECK(UmiComfyNodeCatalogueCreate(&observed,!strcmp(mode,"catalogue-revision")?8U:7U,schema,strlen(schema),NULL,&catalogue)==UMI_STATUS_OK);
    if(!strcmp(mode,"current-settings"))strcpy(current.label,"Changed current label");
    UmiCancellationToken *cancel=NULL;
    if(!strcmp(mode,"cancel")){CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);}
    UmiStatus expected=UMI_STATUS_OK;
    if(!strcmp(mode,"cancel"))expected=UMI_STATUS_CANCELLED;
    if(!strcmp(mode,"current-settings")||!strcmp(mode,"catalogue-settings")||!strcmp(mode,"current-revision")||!strcmp(mode,"catalogue-revision"))expected=UMI_STATUS_BUSY;
    if(!strcmp(mode,"too-many-candidates")||!strcmp(mode,"long-name")||!strcmp(mode,"long-target"))expected=UMI_STATUS_CAPACITY_EXCEEDED;
    CHECK(UmiComfyReferenceInventoryCreate(&current,!strcmp(mode,"current-revision")?8U:7U,prompt,catalogue,cancel,&inventory)==expected);
    umi_cancellation_token_destroy(cancel);
    if(expected!=UMI_STATUS_OK){CHECK(inventory==NULL);goto done;}
    UmiComfyReferenceSummary summary;CHECK(UmiComfyReferenceInventoryInspect(inventory,&summary)==UMI_STATUS_OK);
    CHECK(summary.connection_revision==7U);
    if(fixture!=SIZE_MAX){
        const struct Fixture *f=&fixtures[fixture];
        CHECK(summary.candidate_count==f->count && summary.other_array_count==f->other);
        CHECK(summary.unsupported_slot_count==f->unsupported && summary.missing_target_count==f->missing);
        CHECK(summary.unobserved_output_count==f->unknown && summary.out_of_range_count==f->out);
        CHECK(summary.observed_output_count==f->observed);
        if(!strcmp(mode,"resolved")||!strcmp(mode,"escaped-target")){
            UmiComfyReference item;CHECK(UmiComfyReferenceInventoryAt(inventory,0U,&item)==UMI_STATUS_OK);
            CHECK(!strcmp(item.node_id,"sink")&&!strcmp(item.input_name,"image")&&!strcmp(item.target_id,"source"));
            CHECK(item.slot_known && item.target_found && item.output_count_known && item.output_observed);
            CHECK(item.output_slot==0 && item.declared_output_count==1U && !strcmp(item.output_type,"IMAGE"));
        }else if(!strcmp(mode,"source-order")){
            static const char *nodes[]={"z","z","a"},*inputs[]={"last","first","middle"};
            for(size_t i=0U;i<3U;++i){UmiComfyReference item;CHECK(UmiComfyReferenceInventoryAt(inventory,i,&item)==UMI_STATUS_OK);
                CHECK(!strcmp(item.node_id,nodes[i])&&!strcmp(item.input_name,inputs[i]));}
        }else if(!strcmp(mode,"unicode")){
            UmiComfyReference item;CHECK(UmiComfyReferenceInventoryAt(inventory,0U,&item)==UMI_STATUS_OK);
            CHECK(!strcmp(item.node_id,"接收")&&!strcmp(item.input_name,"图像")&&!strcmp(item.target_id,"源"));
        }else if(f->count==1U){
            UmiComfyReference item;CHECK(UmiComfyReferenceInventoryAt(inventory,0U,&item)==UMI_STATUS_OK);
            if(f->unsupported)CHECK(!item.slot_known && !item.output_observed);
            if(f->missing)CHECK(!item.target_found && !item.output_count_known && !item.output_observed);
            if(f->unknown)CHECK(item.target_found && !item.output_count_known && !item.output_observed);
            if(f->out)CHECK(item.slot_known && item.target_found && item.output_count_known && !item.output_observed);
        }
    }else if(!strcmp(mode,"maximum-candidates")||!strcmp(mode,"maximum-nodes")){
        size_t total=!strcmp(mode,"maximum-nodes")?1024U:8192U;
        CHECK(summary.candidate_count==total && summary.observed_output_count==total);
        UmiComfyReference item;CHECK(UmiComfyReferenceInventoryAt(inventory,total-1U,&item)==UMI_STATUS_OK);
        CHECK(!strcmp(item.node_id,!strcmp(mode,"maximum-nodes")?"n1023":"n"));
    }else if(!strcmp(mode,"maximum-name")){
        UmiComfyReference item;CHECK(UmiComfyReferenceInventoryAt(inventory,0U,&item)==UMI_STATUS_OK);
        CHECK(strlen(item.input_name)==128U && !strcmp(item.input_name,name));
    }else if(!strcmp(mode,"last-output")){
        UmiComfyReference item;CHECK(UmiComfyReferenceInventoryAt(inventory,0U,&item)==UMI_STATUS_OK);
        CHECK(item.output_slot==63 && item.declared_output_count==64U && item.output_observed && !strcmp(item.output_type,"T63"));
    }else if(!strcmp(mode,"owner-lifetime")){
        UmiComfyPromptDestroy(prompt);prompt=NULL;UmiComfyNodeCatalogueDestroy(catalogue);catalogue=NULL;
        UmiComfyReference item;UmiComfyReferenceSummary after;
        CHECK(UmiComfyReferenceInventoryAt(inventory,0U,&item)==UMI_STATUS_OK && !strcmp(item.output_type,"IMAGE"));
        CHECK(UmiComfyReferenceInventoryInspect(inventory,&after)==UMI_STATUS_OK && after.candidate_count==summary.candidate_count);
        CHECK(!memcmp(after.request_sha256,summary.request_sha256,UMI_SHA256_BYTES));
    }else if(!strcmp(mode,"copy-isolation")){
        UmiComfyReference item;CHECK(UmiComfyReferenceInventoryAt(inventory,0U,&item)==UMI_STATUS_OK);
        memset(&item,0xa5,sizeof item);CHECK(UmiComfyReferenceInventoryAt(inventory,0U,&item)==UMI_STATUS_OK && !strcmp(item.target_id,"source"));
        memset(&summary,0xa5,sizeof summary);CHECK(UmiComfyReferenceInventoryInspect(inventory,&summary)==UMI_STATUS_OK && summary.candidate_count==1U);
    }else if(!strcmp(mode,"byte-identities")){
        UmiComfyPromptReview p;UmiComfyNodeCatalogueReview n;const char *body=NULL;
        CHECK(UmiComfyPromptInspect(prompt,&p,&body)==UMI_STATUS_OK && UmiComfyNodeCatalogueInspect(catalogue,&n)==UMI_STATUS_OK);
        CHECK(!memcmp(p.request_sha256,summary.request_sha256,UMI_SHA256_BYTES));
        CHECK(!memcmp(p.connection_sha256,summary.connection_sha256,UMI_SHA256_BYTES));
        CHECK(!memcmp(n.response_sha256,summary.catalogue_sha256,UMI_SHA256_BYTES));
    }else if(!strcmp(mode,"occupied")){
        UmiComfyReferenceInventory *before=inventory;
        CHECK(UmiComfyReferenceInventoryCreate(&c,7U,prompt,catalogue,NULL,&inventory)==UMI_STATUS_INVALID_ARGUMENT && inventory==before);
    }else if(!strcmp(mode,"accessor-refusal")){
        unsigned char before[sizeof summary];memset(&summary,0xa5,sizeof summary);memcpy(before,&summary,sizeof summary);
        CHECK(UmiComfyReferenceInventoryInspect(NULL,&summary)==UMI_STATUS_INVALID_ARGUMENT && !memcmp(before,&summary,sizeof summary));
        UmiComfyReference item;unsigned char prior[sizeof item];memset(&item,0xa5,sizeof item);memcpy(prior,&item,sizeof item);
        CHECK(UmiComfyReferenceInventoryAt(inventory,SIZE_MAX,&item)==UMI_STATUS_NOT_FOUND && !memcmp(prior,&item,sizeof item));
        CHECK(UmiComfyReferenceInventoryAt(inventory,1U,&item)==UMI_STATUS_NOT_FOUND && !memcmp(prior,&item,sizeof item));
        CHECK(UmiComfyReferenceInventoryAt(NULL,0U,&item)==UMI_STATUS_INVALID_ARGUMENT && !memcmp(prior,&item,sizeof item));
    }else if(!strcmp(mode,"nulls")){
        UmiComfyReferenceInventory *none=NULL;
        CHECK(UmiComfyReferenceInventoryCreate(NULL,7U,prompt,catalogue,NULL,&none)==UMI_STATUS_INVALID_ARGUMENT && !none);
        CHECK(UmiComfyReferenceInventoryCreate(&c,7U,NULL,catalogue,NULL,&none)==UMI_STATUS_INVALID_ARGUMENT && !none);
        CHECK(UmiComfyReferenceInventoryCreate(&c,7U,prompt,NULL,NULL,&none)==UMI_STATUS_INVALID_ARGUMENT && !none);
        CHECK(UmiComfyReferenceInventoryCreate(&c,7U,prompt,catalogue,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyReferenceInventoryInspect(inventory,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyReferenceInventoryAt(inventory,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        UmiComfyReferenceInventoryDestroy(NULL);
    }else CHECK(0);
done:
    UmiComfyReferenceInventoryDestroy(inventory);UmiComfyPromptDestroy(prompt);UmiComfyNodeCatalogueDestroy(catalogue);free(owned);return 0;
}
