/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/model_formats/test_safetensors_allocation.c
 *
 * PURPOSE:
 *   Verify actual layout staging cleanup and propagation of shared JSON failures before header publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "safetensors_fixture.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdbool.h>
static bool refuse_range,refuse_parse,refuse_unique;
static size_t range_allocations,live_ranges,live_trees;
static void *tensor_test_calloc(size_t count,size_t size)
{
    ++range_allocations;
    if(refuse_range) return NULL;
    void *memory=calloc(count,size);if(memory!=NULL) ++live_ranges;return memory;
}
static void tensor_test_free(void *memory)
{
    if(memory!=NULL) --live_ranges;
    free(memory);
}
static UmiStatus tensor_test_tree(const void *bytes,size_t length,const UmiJsonTreeLimits *limits,
    const UmiCancellationToken *cancel,UmiJsonTree **out)
{
    if(refuse_parse) { *out=NULL;return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status=UmiJsonTreeCreate(bytes,length,limits,cancel,out);
    if(status==UMI_STATUS_OK) ++live_trees;
    return status;
}
static void tensor_test_destroy(UmiJsonTree *tree)
{
    if(tree!=NULL) --live_trees;
    UmiJsonTreeDestroy(tree);
}
static UmiStatus tensor_test_unique(const UmiJsonTree *tree,int node,const UmiCancellationToken *cancel)
{
    return refuse_unique?UMI_STATUS_OUT_OF_MEMORY:UmiJsonTreeValidateUniqueKeys(tree,node,cancel);
}
UmiStatus UmiTestSafetensorsHeaderInspect(const void *prefix,size_t length,uint64_t file_bytes,
    const UmiCancellationToken *cancel,UmiSafetensorsHeaderInfo *out);
/* Exercise actual format code and range ownership. JSON owner fault coverage
 * lives with the shared tree; injected statuses here verify propagation only. */
#define UmiSafetensorsHeaderInspect UmiTestSafetensorsHeaderInspect
#define UmiJsonTreeCreate tensor_test_tree
#define UmiJsonTreeDestroy tensor_test_destroy
#define UmiJsonTreeValidateUniqueKeys tensor_test_unique
#define calloc tensor_test_calloc
#define free tensor_test_free
#include "../../src/model_formats/safetensors.c"
#undef free
#undef calloc
#undef UmiJsonTreeValidateUniqueKeys
#undef UmiJsonTreeDestroy
#undef UmiJsonTreeCreate
#undef UmiSafetensorsHeaderInspect
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while (0)
static int exercise(const char *mode)
{
    UmiStatus expected=UMI_STATUS_OUT_OF_MEMORY;
    const char *json=tensor_fixture_json;
    if(strcmp(mode,"allocation")==0) refuse_range=true;
    else if(strcmp(mode,"parser")==0) refuse_parse=true;
    else if(strcmp(mode,"unique")==0) refuse_unique=true;
    else if(strcmp(mode,"malformed")==0) {
        json="{\"x\":{\"dtype\":\"F32\",\"shape\":[6],\"data_offsets\":[2,26]}}";
        expected=UMI_STATUS_PARSE_ERROR;
    } else return 2;
    size_t length;unsigned char *prefix=tensor_fixture_prefix(json,0U,&length);
    CHECK(prefix!=NULL);
    UmiSafetensorsHeaderInfo info;memset(&info,0xa5,sizeof(info));
    unsigned char before[sizeof(info)];memcpy(before,&info,sizeof(info));
    CHECK(UmiTestSafetensorsHeaderInspect(prefix,length,length+24U,NULL,&info)==expected);
    CHECK(memcmp(before,&info,sizeof(info))==0 && live_ranges==0U && live_trees==0U);
    CHECK(range_allocations==((refuse_parse || refuse_unique)?0U:1U));
    free(prefix);refuse_range=false;refuse_parse=false;refuse_unique=false;
    prefix=tensor_fixture_prefix(tensor_fixture_json,0U,&length);CHECK(prefix!=NULL);
    CHECK(UmiTestSafetensorsHeaderInspect(prefix,length,length+24U,NULL,&info)==UMI_STATUS_OK);
    CHECK(info.tensors==1U && info.elements==6U && live_ranges==0U && live_trees==0U);
    size_t before_allocations=range_allocations;
    CHECK(UmiTestSafetensorsHeaderInspect(prefix,length,length+24U,NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(range_allocations==before_allocations && live_trees==0U);
    free(prefix);return 0;
}
int main(int argc,char **argv) { return argc==2?exercise(argv[1]):2; }
