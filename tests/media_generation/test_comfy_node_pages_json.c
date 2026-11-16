/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media_generation/test_comfy_node_pages_json.c
 *
 * PURPOSE:
 *   Verify page identities, unknown flags, escaped metadata and transactional output through independent assertions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_node_pages.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;}}while(0)
static int Text(const UmiJsonTree *tree,int object,const char *key,const char *expected)
{
    int field=-1;char text[4096];CHECK(UmiJsonTreeMember(tree,object,key,&field)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeKind(tree,field)==UMI_LANGUAGE_RUNTIME_JSON_STRING);
    CHECK(UmiJsonTreeText(tree,field,text,sizeof text)==UMI_STATUS_OK && !strcmp(text,expected));return 0;
}
static int Number(const UmiJsonTree *tree,int object,const char *key,int64_t expected)
{
    int field=-1;int64_t number=-1;CHECK(UmiJsonTreeMember(tree,object,key,&field)==UMI_STATUS_OK);
    CHECK(UmiJsonTreeInteger(tree,field,&number)==UMI_STATUS_OK && number==expected);return 0;
}
static int Flag(const UmiJsonTree *tree,int object,const char *key,int expected)
{
    int field=-1;CHECK(UmiJsonTreeMember(tree,object,key,&field)==UMI_STATUS_OK);
    if(expected<0){const char *p=NULL;size_t n=0U;
        CHECK(UmiJsonTreeSourceSpan(tree,field,&p,&n)==UMI_STATUS_OK && n==4U && !memcmp(p,"null",4U));
    }else{int value=-1;CHECK(UmiJsonTreeBoolean(tree,field,&value)==UMI_STATUS_OK && value==expected);}
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];
    const char *sample="{\"Z\":{\"display_name\":\"Zulu\",\"category\":\"image/edit\",\"input\":{\"required\":{\"text\":[\"STRING\",{\"default\":\"PRIVATE_DEFAULT\"}]}},\"output\":[\"IMAGE\"],\"output_node\":true,\"private\":\"PRIVATE_EXTENSION\"},\"A\":{\"display_name\":\"Alpha\",\"category\":\"image\",\"input\":{},\"output\":[],\"deprecated\":false},\"B\":{\"display_name\":\"Beta\",\"category\":\"image2\",\"input\":{},\"output\":[],\"experimental\":true,\"api_node\":false},\"C\":{\"display_name\":\"C\\\"\\n東京\",\"category\":\"text\",\"input\":{},\"output\":[],\"output_node\":false}}";
    if(!strcmp(mode,"empty"))sample="{}";
    char *many=NULL;
    if(!strcmp(mode,"maximum-escaped-page")){
        many=malloc(300000U);CHECK(many!=NULL);size_t used=0U;many[used++]='{';
        for(size_t i=0U;i<64U;++i){
            int n=snprintf(many+used,300000U-used,"%s\"N%03zu\":{\"display_name\":\"",i?",":"",i);
            CHECK(n>0 && (size_t)n<300000U-used);used+=(size_t)n;
            for(size_t j=0U;j<256U;++j){memcpy(many+used,"\\u0001",6U);used+=6U;}
            const char *part="\",\"category\":\"";memcpy(many+used,part,strlen(part));used+=strlen(part);
            for(size_t j=0U;j<256U;++j){memcpy(many+used,"\\u0002",6U);used+=6U;}
            part="\",\"input\":{},\"output\":[]}";memcpy(many+used,part,strlen(part));used+=strlen(part);
        }
        memcpy(many+used,"}",2U);sample=many;
    }
    UmiProviderConnection connection={0};strcpy(connection.id,"node-pages");strcpy(connection.label,"Node pages");
    strcpy(connection.provider_id,"comfyui");strcpy(connection.endpoint,"http://127.0.0.1:8188");
    connection.route=UMI_PROVIDER_CONNECTION_LOOPBACK;connection.timeout_ms=30000;connection.enabled=true;
    UmiComfyNodeCatalogue *catalogue=NULL;
    CHECK(UmiComfyNodeCatalogueCreate(&connection,UINT64_MAX,sample,strlen(sample),NULL,&catalogue)==UMI_STATUS_OK);
    char responseDigest[UMI_SHA256_HEX_CAPACITY];CHECK(UmiSha256Buffer(sample,strlen(sample),responseDigest)==UMI_STATUS_OK);
    free(many);UmiComfyNodeQuery query={0};query.order=UMI_COMFY_NODE_ORDER_CLASS;
    if(!strcmp(mode,"search"))strcpy(query.search,"IMAGE");
    if(!strcmp(mode,"category"))strcpy(query.category,"image");
    if(!strcmp(mode,"no-match"))strcpy(query.search,"absent-match");
    if(!strcmp(mode,"required-true"))query.required_true_flags=UMI_COMFY_NODE_OUTPUT_NODE;
    if(!strcmp(mode,"required-false"))query.required_false_flags=UMI_COMFY_NODE_OUTPUT_NODE;
    UmiComfyNodeSelection *selection=NULL;CHECK(UmiComfyNodeSelect(catalogue,&query,NULL,&selection)==UMI_STATUS_OK);
    UmiComfyNodeSelectionReview review;CHECK(UmiComfyNodeSelectionInspect(selection,&review)==UMI_STATUS_OK);
    if(!strcmp(mode,"copied-lifetime")){UmiComfyNodeCatalogueDestroy(catalogue);catalogue=NULL;}
    size_t offset=0U,limit=64U;
    if(!strcmp(mode,"page")){offset=1U;limit=2U;}
    if(!strcmp(mode,"last-page")){offset=3U;limit=2U;}
    if(!strcmp(mode,"end"))offset=4U;
    char *json=malloc(UMI_COMFY_NODE_PAGE_JSON_CAPACITY);CHECK(json!=NULL);size_t length=0U;
    CHECK(UmiComfyNodeSelectionJson(selection,offset,limit,json,UMI_COMFY_NODE_PAGE_JSON_CAPACITY,&length)==UMI_STATUS_OK);
    CHECK(length==strlen(json));UmiJsonTree *tree=NULL;CHECK(UmiJsonTreeCreate(json,length,NULL,NULL,&tree)==UMI_STATUS_OK);
    CHECK(!Text(tree,0,"scope","node-selection-observations"));
    CHECK(!Text(tree,0,"connection_revision","18446744073709551615"));CHECK(!Text(tree,0,"response_sha256",responseDigest));
    char connectionDigest[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(review.catalogue.connection_sha256,connectionDigest);
    CHECK(!Text(tree,0,"connection_sha256",connectionDigest));
    size_t count=review.selected_count-offset;if(count>limit)count=limit;
    CHECK(!Number(tree,0,"source_count",(int64_t)review.source_count));
    CHECK(!Number(tree,0,"selected_count",(int64_t)review.selected_count));
    CHECK(!Number(tree,0,"offset",(int64_t)offset) && !Number(tree,0,"page_count",(int64_t)count));
    CHECK(!Number(tree,0,"next_offset",(int64_t)(offset+count)));
    CHECK(!Number(tree,0,"remaining_count",(int64_t)(review.selected_count-offset-count)));
    int nodes=-1;CHECK(UmiJsonTreeMember(tree,0,"nodes",&nodes)==UMI_STATUS_OK);
    int row=UmiJsonTreeFirst(tree,nodes);size_t rows=0U;
    while(row>=0){CHECK(!Number(tree,row,"selection_index",(int64_t)(offset+rows)));++rows;row=UmiJsonTreeNext(tree,row);}
    CHECK(rows==count);
    if(!strcmp(mode,"summary") || !strcmp(mode,"copied-lifetime") || !strcmp(mode,"page") || !strcmp(mode,"last-page") || !strcmp(mode,"end"))
        CHECK(!Text(tree,0,"selection_sha256","985e214b6802f0a8c9a1fca992f3652e5840639702618a0aeeca601daf02c85d"));
    row=UmiJsonTreeFirst(tree,nodes);
    if(!strcmp(mode,"summary"))CHECK(!Text(tree,row,"class_type","A") && !Number(tree,row,"known_flags",2));
    else if(!strcmp(mode,"unknown-flags"))CHECK(!Flag(tree,row,"output_node",-1) && !Flag(tree,row,"experimental",-1) && !Flag(tree,row,"api_node",-1));
    else if(!strcmp(mode,"known-false"))CHECK(!Flag(tree,row,"deprecated",0));
    else if(!strcmp(mode,"escaping")){
        row=UmiJsonTreeNext(tree,UmiJsonTreeNext(tree,row));CHECK(!Text(tree,row,"display_name","C\"\n東京"));
    }else if(!strcmp(mode,"required-true"))CHECK(count==1U && !Text(tree,row,"class_type","Z") && !Flag(tree,row,"output_node",1));
    else if(!strcmp(mode,"required-false"))CHECK(count==1U && !Text(tree,row,"class_type","C") && !Flag(tree,row,"output_node",0));
    else if(!strcmp(mode,"category"))CHECK(count==2U && !Text(tree,row,"class_type","A") && !Text(tree,UmiJsonTreeNext(tree,row),"class_type","Z"));
    else if(!strcmp(mode,"search"))CHECK(count==3U);
    else if(!strcmp(mode,"empty") || !strcmp(mode,"no-match"))CHECK(count==0U);
    else if(!strcmp(mode,"maximum-escaped-page"))CHECK(count==64U && length<UMI_COMFY_NODE_PAGE_JSON_CAPACITY);
    CHECK(strstr(json,"PRIVATE_DEFAULT")==NULL && strstr(json,"PRIVATE_EXTENSION")==NULL);
    if(!strcmp(mode,"different-order")){
        query.order=UMI_COMFY_NODE_ORDER_SOURCE;UmiComfyNodeSelection *other=NULL;
        CHECK(UmiComfyNodeSelect(catalogue,&query,NULL,&other)==UMI_STATUS_OK);
        char *alternate=malloc(UMI_COMFY_NODE_PAGE_JSON_CAPACITY);CHECK(alternate!=NULL);size_t n=0U;
        CHECK(UmiComfyNodeSelectionJson(other,0U,1U,alternate,UMI_COMFY_NODE_PAGE_JSON_CAPACITY,&n)==UMI_STATUS_OK);
        UmiJsonTree *alt=NULL;CHECK(UmiJsonTreeCreate(alternate,n,NULL,NULL,&alt)==UMI_STATUS_OK);
        int a=-1,b=-1;char ah[65],bh[65];CHECK(UmiJsonTreeMember(tree,0,"selection_sha256",&a)==UMI_STATUS_OK);
        CHECK(UmiJsonTreeMember(alt,0,"selection_sha256",&b)==UMI_STATUS_OK);
        CHECK(UmiJsonTreeText(tree,a,ah,sizeof ah)==UMI_STATUS_OK && UmiJsonTreeText(alt,b,bh,sizeof bh)==UMI_STATUS_OK);
        CHECK(strcmp(ah,bh)!=0);UmiJsonTreeDestroy(alt);free(alternate);UmiComfyNodeSelectionDestroy(other);
    }
    bool refusal=!strcmp(mode,"short-capacity")||!strcmp(mode,"zero-capacity")||!strcmp(mode,"null-owner")||
        !strcmp(mode,"null-output")||!strcmp(mode,"null-length")||!strcmp(mode,"zero-limit")||
        !strcmp(mode,"large-limit")||!strcmp(mode,"beyond")||!strcmp(mode,"huge-offset");
    if(refusal || !strcmp(mode,"exact-capacity")){
        size_t size=length+1U;char *out=malloc(size+8U);CHECK(out!=NULL);memset(out,0x5a,size+8U);size_t n=777U;
        UmiStatus result=UmiComfyNodeSelectionJson(!strcmp(mode,"null-owner")?NULL:selection,
            !strcmp(mode,"beyond")?5U:!strcmp(mode,"huge-offset")?SIZE_MAX:0U,
            !strcmp(mode,"zero-limit")?0U:!strcmp(mode,"large-limit")?65U:64U,
            !strcmp(mode,"null-output")?NULL:out,!strcmp(mode,"zero-capacity")?0U:!strcmp(mode,"short-capacity")?length:size,
            !strcmp(mode,"null-length")?NULL:&n);
        if(refusal){CHECK(result!=UMI_STATUS_OK && n==777U);for(size_t i=0U;i<size+8U;++i)CHECK(out[i]==0x5a);}
        else{CHECK(result==UMI_STATUS_OK && n==length && !strcmp(out,json));for(size_t i=size;i<size+8U;++i)CHECK(out[i]==0x5a);}
        free(out);
    }
    UmiJsonTreeDestroy(tree);free(json);UmiComfyNodeSelectionDestroy(selection);UmiComfyNodeCatalogueDestroy(catalogue);return 0;
}
