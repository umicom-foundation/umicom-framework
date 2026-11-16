/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_palette.c
 *
 * PURPOSE:
 *   Verify deterministic palette selection, unknown flags and copied observation identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_palette.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}}while(0)
static const char fixture[]="{\"Z\":{\"display_name\":\"Same\",\"category\":\"image\",\"input\":{},\"output\":[],\"output_node\":true},\"A\":{\"display_name\":\"Same\",\"category\":\"image/filters\",\"input\":{},\"output\":[],\"output_node\":false,\"deprecated\":false,\"experimental\":true,\"api_node\":false},\"M\":{\"display_name\":\"Mask\",\"category\":\"imagery\",\"input\":{},\"output\":[],\"output_node\":false,\"deprecated\":true,\"experimental\":false,\"api_node\":true},\"Ω\":{\"display_name\":\"écho\",\"category\":\"Image\",\"input\":{},\"output\":[]}}";
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection c={0};strcpy(c.id,"palette-fixture");strcpy(c.provider_id,"comfyui");
    strcpy(c.label,"Palette fixture");strcpy(c.endpoint,"http://127.0.0.1:8188");
    c.enabled=true;c.route=UMI_PROVIDER_CONNECTION_LOOPBACK;c.timeout_ms=30000;return c;
}
static const struct {const char *mode,*search,*category;unsigned on,off;int order;const char *expected;} cases[]={
    {"source","","",0U,0U,0U,"Z,A,M,Ω"},
    {"class","","",0U,0U,1U,"A,M,Z,Ω"},
    {"display","","",0U,0U,2U,"M,A,Z,Ω"},
    {"category-order","","",0U,0U,3U,"Ω,Z,A,M"},
    {"search-display","sAmE","",0U,0U,0U,"Z,A"},
    {"search-class","z","",0U,0U,0U,"Z"},
    {"search-category","IMAge","",0U,0U,0U,"Z,A,M,Ω"},
    {"search-miss","absent","",0U,0U,0U,""},
    {"category-tree","","image",0U,0U,0U,"Z,A"},
    {"category-leaf","","image/filters",0U,0U,0U,"A"},
    {"category-case","","Image",0U,0U,0U,"Ω"},
    {"category-prefix","","imag",0U,0U,0U,""},
    {"known-true","","",1U,0U,0U,"Z"},
    {"known-false","","",0U,2U,0U,"A"},
    {"output-false","","",0U,1U,0U,"A,M"},
    {"combined-flags","","",4U,8U,0U,"A"},
    {"all-true","","",3U,0U,0U,""},
    {"all-false","","",0U,3U,0U,"A"},
    {"unicode","é","",0U,0U,0U,"Ω"},
    {"unicode-case","É","",0U,0U,0U,""}
};
static int Expect(const UmiComfyNodeSelection *selection,const char *expected)
{
    UmiComfyNodeSelectionReview review;CHECK(UmiComfyNodeSelectionInspect(selection,&review)==UMI_STATUS_OK);
    char actual[1024]="";
    for(size_t i=0U;i<review.selected_count;++i){UmiComfyNodeInfo item;CHECK(UmiComfyNodeSelectionAt(selection,i,&item)==UMI_STATUS_OK);
        CHECK(strlen(actual)+strlen(item.class_type)+2U<sizeof actual);if(i)strcat(actual,",");strcat(actual,item.class_type);}
    CHECK(!strcmp(actual,expected));return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];UmiProviderConnection c=Connection();
    UmiComfyNodeCatalogue *source=NULL;CHECK(UmiComfyNodeCatalogueCreate(&c,7U,fixture,strlen(fixture),NULL,&source)==UMI_STATUS_OK);
    UmiComfyNodeQuery query={0};UmiComfyNodeSelection *selection=NULL;
    for(size_t i=0U;i<sizeof cases/sizeof cases[0];++i)if(!strcmp(mode,cases[i].mode)){
        strcpy(query.search,cases[i].search);strcpy(query.category,cases[i].category);query.required_true_flags=cases[i].on;
        query.required_false_flags=cases[i].off;query.order=(UmiComfyNodeOrder)cases[i].order;
        CHECK(UmiComfyNodeSelect(source,&query,NULL,&selection)==UMI_STATUS_OK);
        CHECK(Expect(selection,cases[i].expected)==0);UmiComfyNodeSelectionDestroy(selection);UmiComfyNodeCatalogueDestroy(source);return 0;
    }
    if(!strcmp(mode,"empty") || !strcmp(mode,"maximum")){
        UmiComfyNodeCatalogueDestroy(source);source=NULL;size_t count=!strcmp(mode,"empty")?0U:4096U;
        char *json=malloc(300000U);CHECK(json!=NULL);size_t used=1U;json[0]='{';
        for(size_t i=0U;i<count;++i){int n=snprintf(json+used,300000U-used,"%s\"N%zu\":{\"input\":{},\"output\":[]}",i?",":"",i);CHECK(n>0 && (size_t)n<300000U-used);used+=(size_t)n;}
        strcpy(json+used,"}");CHECK(UmiComfyNodeCatalogueCreate(&c,7U,json,strlen(json),NULL,&source)==UMI_STATUS_OK);free(json);
        CHECK(UmiComfyNodeSelect(source,&query,NULL,&selection)==UMI_STATUS_OK);UmiComfyNodeSelectionReview review;
        CHECK(UmiComfyNodeSelectionInspect(selection,&review)==UMI_STATUS_OK && review.source_count==count && review.selected_count==count);
    }else if(!strncmp(mode,"unterminated-",13U) || !strncmp(mode,"invalid-",8U) || !strcmp(mode,"leading-slash") ||
        !strcmp(mode,"trailing-slash") || !strcmp(mode,"double-slash") || !strcmp(mode,"overlap-flags") || !strcmp(mode,"unknown-flags") || !strcmp(mode,"bad-order")){
        if(!strcmp(mode,"unterminated-search"))memset(query.search,'a',sizeof query.search);
        if(!strcmp(mode,"unterminated-category"))memset(query.category,'a',sizeof query.category);
        if(!strcmp(mode,"invalid-search")){query.search[0]=(char)0xc0;query.search[1]=(char)0xaf;}
        if(!strcmp(mode,"invalid-category")){query.category[0]=(char)0xc0;query.category[1]=(char)0xaf;}
        if(!strcmp(mode,"leading-slash"))strcpy(query.category,"/image");
        if(!strcmp(mode,"trailing-slash"))strcpy(query.category,"image/");
        if(!strcmp(mode,"double-slash"))strcpy(query.category,"image//filter");
        if(!strcmp(mode,"overlap-flags")){query.required_true_flags=1U;query.required_false_flags=1U;}
        if(!strcmp(mode,"unknown-flags"))query.required_false_flags=16U;
        if(!strcmp(mode,"bad-order"))query.order=(UmiComfyNodeOrder)99;
        CHECK(UmiComfyNodeSelect(source,&query,NULL,&selection)==UMI_STATUS_INVALID_ARGUMENT && selection==NULL);
    }else if(!strcmp(mode,"cancel")){
        UmiCancellationToken *cancel=NULL;CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);umi_cancellation_token_request(cancel);
        CHECK(UmiComfyNodeSelect(source,&query,cancel,&selection)==UMI_STATUS_CANCELLED && selection==NULL);umi_cancellation_token_destroy(cancel);
    }else if(!strcmp(mode,"nulls")){
        CHECK(UmiComfyNodeSelect(NULL,&query,NULL,&selection)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyNodeSelect(source,NULL,NULL,&selection)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyNodeSelect(source,&query,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiComfyNodeSelectionCheckCatalogue(NULL,source)==UMI_STATUS_INVALID_ARGUMENT);
    }else{
        CHECK(UmiComfyNodeSelect(source,&query,NULL,&selection)==UMI_STATUS_OK);
        if(!strcmp(mode,"occupied")){
            UmiComfyNodeSelection *before=selection;CHECK(UmiComfyNodeSelect(source,&query,NULL,&selection)==UMI_STATUS_INVALID_ARGUMENT && selection==before);
        }else if(!strcmp(mode,"copy")){
            UmiComfyNodeInfo item;CHECK(UmiComfyNodeSelectionAt(selection,0U,&item)==UMI_STATUS_OK);strcpy(item.class_type,"changed");
            UmiComfyNodeSelectionReview review;CHECK(UmiComfyNodeSelectionInspect(selection,&review)==UMI_STATUS_OK);review.selected_count=0U;
            CHECK(Expect(selection,"Z,A,M,Ω")==0);
        }else if(!strcmp(mode,"owner-lifetime")){
            UmiComfyNodeCatalogueDestroy(source);source=NULL;memset(&query,0xa5,sizeof query);CHECK(Expect(selection,"Z,A,M,Ω")==0);
        }else if(!strcmp(mode,"refusal")){
            UmiComfyNodeInfo out;unsigned char before[sizeof out];memset(&out,0xa5,sizeof out);memcpy(before,&out,sizeof out);
            CHECK(UmiComfyNodeSelectionAt(selection,4U,&out)==UMI_STATUS_NOT_FOUND && !memcmp(before,&out,sizeof out));
            CHECK(UmiComfyNodeSelectionAt(NULL,0U,&out)==UMI_STATUS_INVALID_ARGUMENT && !memcmp(before,&out,sizeof out));
            CHECK(UmiComfyNodeSelectionAt(selection,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
            UmiComfyNodeSelectionReview review;unsigned char prior[sizeof review];memset(&review,0xa5,sizeof review);memcpy(prior,&review,sizeof review);
            CHECK(UmiComfyNodeSelectionInspect(NULL,&review)==UMI_STATUS_INVALID_ARGUMENT && !memcmp(prior,&review,sizeof review));
            CHECK(UmiComfyNodeSelectionInspect(selection,NULL)==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiComfyNodeSelectionCheckCatalogue(selection,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        }else if(!strcmp(mode,"same-catalogue") || !strcmp(mode,"changed-revision") || !strcmp(mode,"changed-settings") || !strcmp(mode,"changed-bytes")){
            UmiComfyNodeCatalogue *other=NULL;uint64_t revision=!strcmp(mode,"changed-revision")?8U:7U;
            if(!strcmp(mode,"changed-settings"))strcpy(c.label,"Different label");
            char spaced[sizeof fixture+2U];strcpy(spaced,fixture);strcat(spaced," ");
            const char *json=!strcmp(mode,"changed-bytes")?spaced:fixture;
            CHECK(UmiComfyNodeCatalogueCreate(&c,revision,json,strlen(json),NULL,&other)==UMI_STATUS_OK);
            CHECK(UmiComfyNodeSelectionCheckCatalogue(selection,other)==(!strcmp(mode,"same-catalogue")?UMI_STATUS_OK:UMI_STATUS_BUSY));
            UmiComfyNodeCatalogueDestroy(other);
        }else CHECK(0);
    }
    UmiComfyNodeSelectionDestroy(selection);UmiComfyNodeCatalogueDestroy(source);return 0;
}
