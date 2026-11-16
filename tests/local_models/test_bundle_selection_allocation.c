/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_bundle_selection_allocation.c
 *
 * PURPOSE:
 *   Verify actual shard-selection staging cleanup and retained output across allocation and publication refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "bundle_fixture.h"
static bool refuse_allocation;
static bool refuse_publication;
static size_t allocations, live_allocations, publications;
static void *selection_calloc(size_t count,size_t size)
{
    ++allocations;
    if(refuse_allocation) return NULL;
    void *memory=calloc(count,size);
    if(memory!=NULL) ++live_allocations;
    return memory;
}
static void selection_free(void *memory)
{
    if(memory!=NULL) --live_allocations;
    free(memory);
}
static UmiStatus selection_publish(const char *repo,const char *revision,
    const UmiModelBundleFile *files,size_t count,UmiModelBundlePlan *out)
{
    ++publications;
    if(refuse_publication) return UMI_STATUS_OUT_OF_MEMORY;
    return UmiModelBundlePlanCreate(repo,revision,files,count,out);
}
UmiStatus UmiTestSelectGguf(const UmiModelBundlePlan *catalogue,
    size_t selected_index,UmiModelBundlePlan *out_plan);
/* Exercise the real selection body with observable allocation ownership. The
 * shared constructor's own allocation failures have separate bundle coverage. */
#define calloc selection_calloc
#define free selection_free
#define UmiModelBundlePlanCreate selection_publish
#define UmiModelBundleSelectGguf UmiTestSelectGguf
#include "../../src/local_models/bundle_selection.c"
#undef UmiModelBundleSelectGguf
#undef UmiModelBundlePlanCreate
#undef free
#undef calloc
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while (0)

static int exercise(BundleFixture *f,UmiModelBundlePlan *catalogue,const char *mode)
{
    f->files[0]=bundle_fixture_file("model-00002-of-00002.gguf",UMI_MODEL_ARTIFACT_GGUF,24U);
    f->files[1]=bundle_fixture_file("model-00001-of-00002.gguf",UMI_MODEL_ARTIFACT_GGUF,25U);
    CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,2U,catalogue)==UMI_STATUS_OK);
    UmiStatus expected=UMI_STATUS_OUT_OF_MEMORY;
    if(strcmp(mode,"allocation")==0) refuse_allocation=true;
    else if(strcmp(mode,"publication")==0) refuse_publication=true;
    else if(strcmp(mode,"missing")==0) {
        strcpy(catalogue->files[1].filename,"other.gguf");expected=UMI_STATUS_NOT_FOUND;
    } else if(strcmp(mode,"invalid")==0) {
        catalogue->revision[0]='x';expected=UMI_STATUS_INVALID_ARGUMENT;
    } else return 2;
    CHECK(UmiTestSelectGguf(catalogue,0U,&f->plan)==expected);
    CHECK(memcmp(&f->plan,f->before,sizeof(f->plan))==0);
    CHECK(live_allocations==0U);
    CHECK(allocations==(strcmp(mode,"invalid")==0?0U:1U));
    CHECK(publications==(strcmp(mode,"publication")==0?1U:0U));
    refuse_allocation=false;refuse_publication=false;
    CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,2U,catalogue)==UMI_STATUS_OK);
    CHECK(UmiTestSelectGguf(catalogue,0U,&f->plan)==UMI_STATUS_OK);
    CHECK(live_allocations==0U && f->plan.count==2U && f->plan.total_bytes==49U);
    CHECK(strcmp(f->plan.files[0].filename,"model-00001-of-00002.gguf")==0);
    CHECK(UmiTestSelectGguf(&f->plan,1U,&f->plan)==UMI_STATUS_OK);
    CHECK(live_allocations==0U && f->plan.total_bytes==49U);
    size_t before=allocations;
    CHECK(UmiTestSelectGguf(catalogue,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(allocations==before);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    BundleFixture *fixture=bundle_fixture_create();
    UmiModelBundlePlan *catalogue=calloc(1U,sizeof(*catalogue));
    if(fixture==NULL || catalogue==NULL) { free(fixture);free(catalogue);return 2; }
    int result=exercise(fixture,catalogue,argv[1]);free(fixture);free(catalogue);return result;
}
