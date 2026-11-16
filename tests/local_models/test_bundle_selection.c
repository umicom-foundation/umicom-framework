/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_bundle_selection.c
 *
 * PURPOSE:
 *   Verify complete numbered GGUF selection, exact identity, bounded input and preserved publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "bundle_fixture.h"
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr,"line %d: %s\n",__LINE__,#condition); return 1; } } while (0)

static void shard_fixture(BundleFixture *f)
{
    f->files[0]=bundle_fixture_file("weights/model-00003-of-00003.gguf",UMI_MODEL_ARTIFACT_GGUF,27U);
    f->files[1]=bundle_fixture_file("config.json",UMI_MODEL_ARTIFACT_JSON,2U);
    f->files[2]=bundle_fixture_file("weights/model-00001-of-00003.gguf",UMI_MODEL_ARTIFACT_GGUF,25U);
    f->files[3]=bundle_fixture_file("weights/model-00002-of-00003.gguf",UMI_MODEL_ARTIFACT_GGUF,26U);
    f->files[0].sha256[0]='c'; f->files[2].sha256[0]='a'; f->files[3].sha256[0]='b';
}
static int expect_refusal(BundleFixture *f,const UmiModelBundlePlan *catalogue,
    size_t selected,UmiStatus status)
{
    CHECK(UmiModelBundleSelectGguf(catalogue,selected,&f->plan)==status);
    CHECK(memcmp(&f->plan,f->before,sizeof(f->plan))==0);
    return 0;
}
static int check_order(const UmiModelBundlePlan *plan)
{
    CHECK(plan->count==3U && plan->total_bytes==78U);
    CHECK(strcmp(plan->repository,bundle_repository)==0);
    CHECK(strcmp(plan->revision,bundle_revision)==0);
    for(size_t part=1U;part<=3U;++part) {
        char expected[80];
        snprintf(expected,sizeof(expected),"weights/model-%05zu-of-00003.gguf",part);
        CHECK(strcmp(plan->files[part-1U].filename,expected)==0);
        CHECK(plan->files[part-1U].bytes==24U+part);
        CHECK(plan->files[part-1U].sha256[0]==(char)('a'+part-1U));
    }
    CHECK(UmiModelBundlePlanValidate(plan)==UMI_STATUS_OK);
    return 0;
}
static int exercise(BundleFixture *f,UmiModelBundlePlan *catalogue,const char *mode)
{
    shard_fixture(f);
    const char *invalid_name=NULL;
    if(strcmp(mode,"part-zero")==0) invalid_name="model-00000-of-00003.gguf";
    else if(strcmp(mode,"total-zero")==0) invalid_name="model-00001-of-00000.gguf";
    else if(strcmp(mode,"part-exceeds-total")==0) invalid_name="model-00004-of-00003.gguf";
    else if(strcmp(mode,"part-nondigit")==0) invalid_name="model-0000x-of-00003.gguf";
    else if(strcmp(mode,"total-nondigit")==0) invalid_name="model-00001-of-0000x.gguf";
    else if(strcmp(mode,"empty-prefix")==0) invalid_name="-00001-of-00003.gguf";
    else if(strcmp(mode,"empty-basename-prefix")==0) invalid_name="weights/-00001-of-00003.gguf";
    else if(strcmp(mode,"excessive-total")==0) invalid_name="model-00001-of-99999.gguf";
    if(invalid_name!=NULL) {
        f->files[0]=bundle_fixture_file(invalid_name,UMI_MODEL_ARTIFACT_GGUF,24U);
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,4U,catalogue)==UMI_STATUS_OK);
        return expect_refusal(f,catalogue,0U,strcmp(mode,"excessive-total")==0?
            UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_INVALID_ARGUMENT);
    }
    if(strcmp(mode,"missing-companion")==0 || strcmp(mode,"wrong-total")==0 || strcmp(mode,"exact-case")==0) {
        const char *replacement=strcmp(mode,"missing-companion")==0?"unrelated.gguf":
            strcmp(mode,"wrong-total")==0?"weights/model-00002-of-00004.gguf":"Weights/model-00002-of-00003.gguf";
        f->files[3]=bundle_fixture_file(replacement,UMI_MODEL_ARTIFACT_GGUF,26U);
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,4U,catalogue)==UMI_STATUS_OK);
        return expect_refusal(f,catalogue,0U,UMI_STATUS_NOT_FOUND);
    }
    if(strcmp(mode,"single")==0 || strcmp(mode,"nonstandard-width")==0 || strcmp(mode,"one-part")==0) {
        const char *name=strcmp(mode,"single")==0?"weights/model.gguf":
            strcmp(mode,"nonstandard-width")==0?"weights/model-1-of-3.gguf":"weights/model-00001-of-00001.gguf";
        f->files[0]=bundle_fixture_file(name,UMI_MODEL_ARTIFACT_GGUF,24U);
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,4U,catalogue)==UMI_STATUS_OK);
        CHECK(UmiModelBundleSelectGguf(catalogue,0U,&f->plan)==UMI_STATUS_OK);
        CHECK(f->plan.count==1U && f->plan.total_bytes==24U);
        CHECK(strcmp(f->plan.files[0].filename,name)==0);
        return 0;
    }
    if(strcmp(mode,"capacity")==0) {
        for(size_t i=0U;i<UMI_MODEL_BUNDLE_MAX_FILES;++i) {
            char name[80];snprintf(name,sizeof(name),"model-%05zu-of-%05u.gguf",
                UMI_MODEL_BUNDLE_MAX_FILES-i,UMI_MODEL_BUNDLE_MAX_FILES);
            f->files[i]=bundle_fixture_file(name,UMI_MODEL_ARTIFACT_GGUF,24U);
        }
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,
            UMI_MODEL_BUNDLE_MAX_FILES,catalogue)==UMI_STATUS_OK);
        CHECK(UmiModelBundleSelectGguf(catalogue,0U,&f->plan)==UMI_STATUS_OK);
        CHECK(f->plan.count==UMI_MODEL_BUNDLE_MAX_FILES && f->plan.total_bytes==128U*24U);
        CHECK(strcmp(f->plan.files[0].filename,"model-00001-of-00128.gguf")==0);
        CHECK(strcmp(f->plan.files[127].filename,"model-00128-of-00128.gguf")==0);
        return 0;
    }
    if(strcmp(mode,"maximum-name")==0) {
        char prefix[492];memset(prefix,'a',sizeof(prefix)-1U);prefix[491]='\0';
        for(size_t i=1U;i<300U;i+=2U) prefix[i]='/';
        f->files[0]=bundle_fixture_file("model.gguf",UMI_MODEL_ARTIFACT_GGUF,24U);
        snprintf(f->files[0].filename,sizeof(f->files[0].filename),"%s-00001-of-00001.gguf",prefix);
        CHECK(strlen(f->files[0].filename)==511U);
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,1U,catalogue)==UMI_STATUS_OK);
        CHECK(UmiModelBundleSelectGguf(catalogue,0U,&f->plan)==UMI_STATUS_OK);
        CHECK(strcmp(f->plan.files[0].filename,f->files[0].filename)==0);
        return 0;
    }
    CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,4U,catalogue)==UMI_STATUS_OK);
    if(strcmp(mode,"first")==0 || strcmp(mode,"middle")==0 || strcmp(mode,"last")==0) {
        size_t selected=strcmp(mode,"first")==0?2U:strcmp(mode,"middle")==0?3U:0U;
        CHECK(UmiModelBundleSelectGguf(catalogue,selected,&f->plan)==UMI_STATUS_OK);
        return check_order(&f->plan);
    }
    if(strcmp(mode,"in-place")==0) {
        CHECK(UmiModelBundleSelectGguf(catalogue,0U,catalogue)==UMI_STATUS_OK);
        return check_order(catalogue);
    }
    if(strcmp(mode,"in-place-refusal")==0) {
        strcpy(catalogue->files[3].filename,"other.gguf");
        memcpy(f->before,catalogue,sizeof(*catalogue));
        CHECK(UmiModelBundleSelectGguf(catalogue,0U,catalogue)==UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(f->before,catalogue,sizeof(*catalogue))==0);
        return 0;
    }
    if(strcmp(mode,"unsupported")==0) return expect_refusal(f,catalogue,1U,UMI_STATUS_NOT_IMPLEMENTED);
    if(strcmp(mode,"index")==0) return expect_refusal(f,catalogue,SIZE_MAX,UMI_STATUS_NOT_FOUND);
    if(strcmp(mode,"nulls")==0) {
        CHECK(expect_refusal(f,NULL,0U,UMI_STATUS_INVALID_ARGUMENT)==0);
        CHECK(UmiModelBundleSelectGguf(catalogue,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if(strcmp(mode,"unselected-invalid")==0) {
        catalogue->files[1].sha256[0]='z';
        return expect_refusal(f,catalogue,0U,UMI_STATUS_INVALID_ARGUMENT);
    }
    if(strcmp(mode,"edited-total")==0) {
        ++catalogue->total_bytes;
        return expect_refusal(f,catalogue,0U,UMI_STATUS_INVALID_STATE);
    }
    if(strcmp(mode,"unterminated-name")==0) {
        memset(catalogue->files[0].filename,'x',sizeof(catalogue->files[0].filename));
        return expect_refusal(f,catalogue,0U,UMI_STATUS_INVALID_ARGUMENT);
    }
    if(strcmp(mode,"edited-duplicate")==0) {
        catalogue->files[3]=catalogue->files[0];
        return expect_refusal(f,catalogue,0U,UMI_STATUS_ALREADY_EXISTS);
    }
    if(strcmp(mode,"oversized-catalogue")==0) {
        catalogue->count=SIZE_MAX;
        return expect_refusal(f,catalogue,0U,UMI_STATUS_CAPACITY_EXCEEDED);
    }
    return 2;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    BundleFixture *fixture=bundle_fixture_create();
    UmiModelBundlePlan *catalogue=calloc(1U,sizeof(*catalogue));
    if(fixture==NULL || catalogue==NULL) { free(fixture);free(catalogue);return 2; }
    int result=exercise(fixture,catalogue,argv[1]);
    free(fixture);free(catalogue);return result;
}
