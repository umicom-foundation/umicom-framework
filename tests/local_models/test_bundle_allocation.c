/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_bundle_allocation.c
 *
 * PURPOSE:
 *   Verify actual bundle constructor staging, allocation recovery and borrowed-input ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "bundle_fixture.h"
static bool fail_allocation;
static size_t allocations;
static size_t live_allocations;
static void *bundle_test_malloc(size_t size)
{
    ++allocations;
    if(fail_allocation) return NULL;
    void *memory=malloc(size);
    if(memory!=NULL) ++live_allocations;
    return memory;
}
static void bundle_test_free(void *memory)
{
    if(memory!=NULL) --live_allocations;
    free(memory);
}
UmiStatus UmiTestBundleCreate(const char *repository,const char *revision,
    const UmiModelBundleFile *files,size_t count,UmiModelBundlePlan *plan);
UmiStatus UmiTestBundleValidate(const UmiModelBundlePlan *plan);
UmiStatus UmiTestBundleSource(const UmiModelBundlePlan *plan,size_t index,UmiModelSource *source);
UmiStatus UmiTestBundleUrl(const UmiModelBundlePlan *plan,size_t index,char *url,size_t capacity);
#define malloc bundle_test_malloc
#define free bundle_test_free
#define UmiModelBundlePlanCreate UmiTestBundleCreate
#define UmiModelBundlePlanValidate UmiTestBundleValidate
#define UmiModelBundleGgufSourceAt UmiTestBundleSource
#define UmiModelBundleUrlAt UmiTestBundleUrl
#include "../../src/local_models/bundle.c"
#undef UmiModelBundleUrlAt
#undef UmiModelBundleGgufSourceAt
#undef UmiModelBundlePlanValidate
#undef UmiModelBundlePlanCreate
#undef free
#undef malloc
#define CHECK(condition) do { if(!(condition)){fprintf(stderr,"line %d: %s\n",__LINE__,#condition);return 1;} } while(0)

static int exercise(BundleFixture *f,const char *mode)
{
    if(strcmp(mode,"allocation")==0) {
        fail_allocation=true;
        CHECK(UmiTestBundleCreate(bundle_repository,bundle_revision,f->files,3U,&f->plan)==UMI_STATUS_OUT_OF_MEMORY);
    } else if(strcmp(mode,"invalid")==0) {
        CHECK(UmiTestBundleCreate(bundle_repository,"main",f->files,3U,&f->plan)==UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    CHECK(allocations==1U && live_allocations==0U);
    CHECK(memcmp(f->before,&f->plan,sizeof(f->plan))==0);
    fail_allocation=false;
    CHECK(UmiTestBundleCreate(bundle_repository,bundle_revision,f->files,3U,&f->plan)==UMI_STATUS_OK);
    CHECK(allocations==2U && live_allocations==0U);
    CHECK(UmiTestBundleCreate(f->plan.repository,f->plan.revision,f->plan.files,
        f->plan.count,&f->plan)==UMI_STATUS_OK);
    CHECK(allocations==3U && live_allocations==0U && f->plan.total_bytes==36U);
    CHECK(UmiTestBundleCreate(bundle_repository,bundle_revision,f->files,3U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    CHECK(allocations==3U);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    BundleFixture *fixture=bundle_fixture_create();
    if(fixture==NULL) return 2;
    int result=exercise(fixture,argv[1]);free(fixture);return result;
}
