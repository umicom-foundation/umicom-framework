/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/json_tree/test_unique_keys.c
 *
 * PURPOSE:
 *   Verify whole-subtree duplicate-name admission, decoded identity, bounded traversal and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/json_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while (0)
typedef struct UniqueVector { const char *mode, *json; UmiStatus expected; } UniqueVector;
static const UniqueVector vectors[] = {
    {"empty-object","{}",UMI_STATUS_OK},
    {"empty-array","[]",UMI_STATUS_OK},
    {"primitive","17",UMI_STATUS_OK},
    {"string","\"a\"",UMI_STATUS_OK},
    {"unique","{\"b\":2,\"a\":1}",UMI_STATUS_OK},
    {"nested","{\"a\":[{\"b\":1},{\"c\":2}]}",UMI_STATUS_OK},
    {"separate-objects","[{\"a\":1},{\"a\":2}]",UMI_STATUS_OK},
    {"case-distinct","{\"A\":1,\"a\":2}",UMI_STATUS_OK},
    {"unicode-distinct","{\"\\u00e9\":1,\"e\\u0301\":2}",UMI_STATUS_OK},
    {"duplicate","{\"a\":1,\"a\":2}",UMI_STATUS_ALREADY_EXISTS},
    {"escaped-duplicate","{\"a\":1,\"\\u0061\":2}",UMI_STATUS_ALREADY_EXISTS},
    {"unicode-duplicate","{\"é\":1,\"\\u00e9\":2}",UMI_STATUS_ALREADY_EXISTS},
    {"metadata-duplicate","{\"__metadata__\":{\"name\":\"one\",\"name\":\"two\"}}",UMI_STATUS_ALREADY_EXISTS},
    {"array-duplicate","[{\"a\":[{\"b\":1,\"b\":2}]}]",UMI_STATUS_ALREADY_EXISTS},
    {"late-duplicate","{\"a\":false,\"z\":[1,2,{\"x\":1,\"x\":2}]}",UMI_STATUS_ALREADY_EXISTS},
    {"empty-key","{\"\":1,\"a\":2}",UMI_STATUS_OK},
    {"duplicate-empty-key","{\"\":1,\"\":2}",UMI_STATUS_ALREADY_EXISTS}
};
static int exercise(const char *mode)
{
    const char *json=NULL;
    UmiStatus expected=UMI_STATUS_OK;
    for(size_t i=0U;i<sizeof(vectors)/sizeof(vectors[0]);++i)
        if(strcmp(mode,vectors[i].mode)==0) { json=vectors[i].json;expected=vectors[i].expected; }
    char *owned=NULL;
    if(strcmp(mode,"subtree")==0 || strcmp(mode,"outside-subtree")==0 || strcmp(mode,"subtree-duplicate")==0) {
        json=strcmp(mode,"subtree-duplicate")==0?"{\"selected\":{\"x\":1,\"x\":2}}":
            strcmp(mode,"outside-subtree")==0?"{\"outside\":{\"x\":1,\"x\":2},\"selected\":{\"x\":3}}":
            "{\"selected\":[{\"x\":1}]}";
        if(strcmp(mode,"subtree-duplicate")==0) expected=UMI_STATUS_ALREADY_EXISTS;
    }
    if(strcmp(mode,"null-tree")==0 || strcmp(mode,"negative-node")==0 || strcmp(mode,"outside-node")==0) {
        json="{}";expected=UMI_STATUS_INVALID_ARGUMENT;
    }
    if(strcmp(mode,"cancel")==0 || strcmp(mode,"cancel-empty")==0 || strcmp(mode,"cancel-reset")==0) {
        json=strcmp(mode,"cancel-empty")==0?"{}":"{\"a\":[{\"b\":1}]}";
        expected=strcmp(mode,"cancel-reset")==0?UMI_STATUS_OK:UMI_STATUS_CANCELLED;
    }
    if(strcmp(mode,"long-key")==0 || strcmp(mode,"long-duplicate")==0) {
        owned=malloc(22032U);CHECK(owned!=NULL);owned[0]='{';owned[1]='"';
        memset(owned+2,'x',10000U);strcpy(owned+10002U,"\":1");
        if(strcmp(mode,"long-duplicate")==0) {
            strcpy(owned+10005U,",\"");memset(owned+10007U,'x',10000U);
            strcpy(owned+20007U,"\":2}");expected=UMI_STATUS_ALREADY_EXISTS;
        } else strcpy(owned+10005U,"}");
        json=owned;
    }
    if(strcmp(mode,"deep")==0) {
        owned=malloc(258U);CHECK(owned!=NULL);memset(owned,'[',128U);
        owned[128]='0';memset(owned+129,']',128U);owned[257]='\0';json=owned;
    }
    if(strcmp(mode,"wide")==0) {
        owned=malloc(16384U);CHECK(owned!=NULL);owned[0]='{';size_t used=1U;
        for(size_t i=0U;i<512U;++i) {
            int n=snprintf(owned+used,16384U-used,"%s\"field%zu\":%zu",i?",":"",511U-i,i);
            CHECK(n>0 && (size_t)n<16384U-used);used+=(size_t)n;
        }
        strcpy(owned+used,"}");json=owned;
    }
    CHECK(json!=NULL);
    UmiJsonTree *tree=NULL;
    CHECK(UmiJsonTreeCreate(json,strlen(json),NULL,NULL,&tree)==UMI_STATUS_OK);
    free(owned);
    int node=0;
    if(strcmp(mode,"subtree")==0 || strcmp(mode,"outside-subtree")==0 || strcmp(mode,"subtree-duplicate")==0)
        CHECK(UmiJsonTreeMember(tree,0,"selected",&node)==UMI_STATUS_OK);
    if(strcmp(mode,"negative-node")==0) node=-1;
    if(strcmp(mode,"outside-node")==0) node=999999;
    const char *before=NULL;size_t before_length=0U;
    CHECK(UmiJsonTreeSourceSpan(tree,0,&before,&before_length)==UMI_STATUS_OK);
    char *snapshot=malloc(before_length);CHECK(snapshot!=NULL);memcpy(snapshot,before,before_length);
    UmiCancellationToken *cancel=NULL;
    CHECK(umi_cancellation_token_create(&cancel)==UMI_STATUS_OK);
    if(strncmp(mode,"cancel",6U)==0) umi_cancellation_token_request(cancel);
    if(strcmp(mode,"cancel-reset")==0) umi_cancellation_token_reset(cancel);
    CHECK(UmiJsonTreeValidateUniqueKeys(strcmp(mode,"null-tree")==0?NULL:tree,node,cancel)==expected);
    const char *after=NULL;size_t after_length=0U;
    CHECK(UmiJsonTreeSourceSpan(tree,0,&after,&after_length)==UMI_STATUS_OK);
    CHECK(after_length==before_length && memcmp(after,snapshot,after_length)==0);
    free(snapshot);umi_cancellation_token_destroy(cancel);UmiJsonTreeDestroy(tree);return 0;
}
int main(int argc,char **argv) { return argc==2?exercise(argv[1]):2; }
