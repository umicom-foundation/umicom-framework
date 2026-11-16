/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_models/test_bundle.c
 *
 * PURPOSE:
 *   Exercise exact multi-file model identity, path collisions, bounds and preserved publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "bundle_fixture.h"
#define CHECK(condition) do { if(!(condition)){fprintf(stderr,"line %d: %s\n",__LINE__,#condition);return 1;} } while(0)

static int refused(BundleFixture *f, const char *repo, const char *revision,
    const UmiModelBundleFile *files, size_t count, UmiStatus expected)
{
    CHECK(UmiModelBundlePlanCreate(repo,revision,files,count,&f->plan)==expected);
    CHECK(memcmp(&f->plan,f->before,sizeof(f->plan))==0);
    return 0;
}
static int run_case(BundleFixture *f, const char *name)
{
    if(strcmp(name,"valid")==0) {
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,3U,&f->plan)==UMI_STATUS_OK);
        CHECK(f->plan.count==3U && f->plan.total_bytes==36U);
        CHECK(UmiModelBundlePlanValidate(&f->plan)==UMI_STATUS_OK);
        CHECK(strcmp(f->plan.files[1].filename,"unet/model.safetensors")==0);
        return 0;
    }
    if(strcmp(name,"repository")==0) {
        const char *bad[]={NULL,"","owner","/repo","owner/","a/b/c","a/../b","a/b?download=true"};
        for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i)
            CHECK(refused(f,bad[i],bundle_revision,f->files,3U,UMI_STATUS_INVALID_ARGUMENT)==0);
        return 0;
    }
    if(strcmp(name,"revision")==0) {
        const char *bad[]={NULL,"main","0123456789abcdef0123456789abcdef0123456","0123456789abcdef0123456789abcdef0123456G"};
        for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i)
            CHECK(refused(f,bundle_repository,bad[i],f->files,3U,UMI_STATUS_INVALID_ARGUMENT)==0);
        return 0;
    }
    if(strcmp(name,"count")==0) {
        CHECK(refused(f,bundle_repository,bundle_revision,f->files,0U,UMI_STATUS_INVALID_ARGUMENT)==0);
        CHECK(refused(f,bundle_repository,bundle_revision,f->files,SIZE_MAX,UMI_STATUS_CAPACITY_EXCEEDED)==0);
        CHECK(refused(f,bundle_repository,bundle_revision,NULL,1U,UMI_STATUS_INVALID_ARGUMENT)==0);
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,3U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if(strcmp(name,"digest")==0 || strcmp(name,"filename-text")==0) {
        if(strcmp(name,"digest")==0) memset(f->files[2].sha256,'a',sizeof(f->files[2].sha256));
        else memset(f->files[2].filename,'x',sizeof(f->files[2].filename));
        return refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_INVALID_ARGUMENT);
    }
    if(strcmp(name,"paths")==0) {
        const char *bad[]={"../model.json","x/../model.json","/model.json","x//model.json",
            "x\\model.json","C:model.json","model.json?x","x/%2e%2e/model.json","x/model.json#tag"};
        for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i) {
            snprintf(f->files[2].filename,sizeof(f->files[2].filename),"%s",bad[i]);
            CHECK(refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_INVALID_ARGUMENT)==0);
        }
        return 0;
    }
    if(strcmp(name,"formats")==0) {
        const char *names[]={"model.gguf","model.safetensors","config.json","tokens.txt",
            "tokenizer.model","tokens.tiktoken","tokens.vocab","tokens.merges",
            "README.md","LICENSE","Licence.md"};
        UmiModelArtifactFormat formats[]={UMI_MODEL_ARTIFACT_GGUF,UMI_MODEL_ARTIFACT_SAFETENSORS,
            UMI_MODEL_ARTIFACT_JSON,UMI_MODEL_ARTIFACT_TEXT,UMI_MODEL_ARTIFACT_TOKENIZER,
            UMI_MODEL_ARTIFACT_TOKENIZER,UMI_MODEL_ARTIFACT_TOKENIZER,UMI_MODEL_ARTIFACT_TOKENIZER,
            UMI_MODEL_ARTIFACT_DOCUMENTATION,UMI_MODEL_ARTIFACT_DOCUMENTATION,UMI_MODEL_ARTIFACT_DOCUMENTATION};
        for(size_t i=0U;i<sizeof(names)/sizeof(names[0]);++i)
            f->files[i]=bundle_fixture_file(names[i],formats[i],24U);
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,
            sizeof(names)/sizeof(names[0]),&f->plan)==UMI_STATUS_OK);
        return 0;
    }
    if(strcmp(name,"executables")==0) {
        const char *bad[]={"config.py","model.bin","model.pkl","module.dll","model.pt","model.safetensors.exe"};
        for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i) {
            f->files[2]=bundle_fixture_file(bad[i],UMI_MODEL_ARTIFACT_JSON,24U);
            CHECK(refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_INVALID_ARGUMENT)==0);
        }
        return 0;
    }
    if(strcmp(name,"format-mismatch")==0) {
        f->files[0].format=UMI_MODEL_ARTIFACT_SAFETENSORS;
        CHECK(refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_INVALID_ARGUMENT)==0);
        f->files[0].format=(UmiModelArtifactFormat)999;
        return refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_INVALID_ARGUMENT);
    }
    if(strcmp(name,"size")==0) {
        uint64_t bad[]={0U,23U,UINT64_MAX};
        for(size_t i=0U;i<sizeof(bad)/sizeof(bad[0]);++i) {
            f->files[0].bytes=bad[i];
            CHECK(refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_INVALID_ARGUMENT)==0);
        }
        f->files[0].bytes=24U;f->files[1].bytes=9U;
        return refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_INVALID_ARGUMENT);
    }
    if(strcmp(name,"total")==0) {
        f->files[0].bytes=UMI_MODEL_MAX_BYTES-12U;
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,3U,&f->plan)==UMI_STATUS_OK);
        CHECK(f->plan.total_bytes==UMI_MODEL_MAX_BYTES);
        memcpy(f->before,&f->plan,sizeof(f->plan));
        ++f->files[0].bytes;
        return refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_CAPACITY_EXCEEDED);
    }
    if(strcmp(name,"duplicate")==0 || strcmp(name,"case-collision")==0) {
        f->files[2]=bundle_fixture_file(strcmp(name,"duplicate")==0?"model.gguf":"MODEL.gguf",
            UMI_MODEL_ARTIFACT_GGUF,24U);
        return refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_ALREADY_EXISTS);
    }
    if(strcmp(name,"prefix-collision")==0) {
        f->files[0]=bundle_fixture_file("dir.json",UMI_MODEL_ARTIFACT_JSON,2U);
        f->files[1]=bundle_fixture_file("DIR.json/child.json",UMI_MODEL_ARTIFACT_JSON,2U);
        CHECK(refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_ALREADY_EXISTS)==0);
        UmiModelBundleFile temporary=f->files[0];f->files[0]=f->files[1];f->files[1]=temporary;
        return refused(f,bundle_repository,bundle_revision,f->files,3U,UMI_STATUS_ALREADY_EXISTS);
    }
    if(strcmp(name,"capacity")==0) {
        for(size_t i=0U;i<UMI_MODEL_BUNDLE_MAX_FILES;++i) {
            f->files[i]=bundle_fixture_file("part.json",UMI_MODEL_ARTIFACT_JSON,1U);
            snprintf(f->files[i].filename,sizeof(f->files[i].filename),"parts/part-%03zu.json",i);
        }
        CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,
            UMI_MODEL_BUNDLE_MAX_FILES,&f->plan)==UMI_STATUS_OK);
        CHECK(f->plan.count==UMI_MODEL_BUNDLE_MAX_FILES && f->plan.total_bytes==UMI_MODEL_BUNDLE_MAX_FILES);
        return 0;
    }
    CHECK(UmiModelBundlePlanCreate(bundle_repository,bundle_revision,f->files,3U,&f->plan)==UMI_STATUS_OK);
    if(strcmp(name,"borrowed")==0) {
        CHECK(UmiModelBundlePlanCreate(f->plan.repository,f->plan.revision,
            f->plan.files,f->plan.count,&f->plan)==UMI_STATUS_OK);
        CHECK(f->plan.total_bytes==36U && strcmp(f->plan.files[0].filename,"model.gguf")==0);
        return 0;
    }
    if(strcmp(name,"edited-total")==0) {
        ++f->plan.total_bytes;
        CHECK(UmiModelBundlePlanValidate(&f->plan)==UMI_STATUS_INVALID_STATE);
        return 0;
    }
    if(strcmp(name,"late-invalid")==0) {
        f->plan.files[2].sha256[0]='z';
        UmiModelSource source;memset(&source,0xa5,sizeof(source));
        unsigned char before[sizeof(source)];memcpy(before,&source,sizeof(source));
        CHECK(UmiModelBundleGgufSourceAt(&f->plan,0U,&source)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before,&source,sizeof(source))==0);
        char url[UMI_MODEL_URL_CAPACITY];memset(url,'x',sizeof(url));
        CHECK(UmiModelBundleUrlAt(&f->plan,0U,url,sizeof(url))==UMI_STATUS_INVALID_ARGUMENT);
        for(size_t i=0U;i<sizeof(url);++i) CHECK(url[i]=='x');
        return 0;
    }
    if(strcmp(name,"source")==0) {
        UmiModelSource source;
        CHECK(UmiModelBundleGgufSourceAt(&f->plan,0U,&source)==UMI_STATUS_OK);
        CHECK(UmiModelSourceValidate(&source)==UMI_STATUS_OK && source.bytes==24U);
        CHECK(strcmp(source.revision,bundle_revision)==0 && strcmp(source.sha256,bundle_digest)==0);
        unsigned char before[sizeof(source)];memcpy(before,&source,sizeof(source));
        CHECK(UmiModelBundleGgufSourceAt(&f->plan,1U,&source)==UMI_STATUS_NOT_IMPLEMENTED);
        CHECK(memcmp(before,&source,sizeof(source))==0);
        CHECK(UmiModelBundleGgufSourceAt(&f->plan,SIZE_MAX,&source)==UMI_STATUS_NOT_FOUND);
        CHECK(memcmp(before,&source,sizeof(source))==0);
        return 0;
    }
    if(strcmp(name,"url")==0) {
        char url[UMI_MODEL_URL_CAPACITY];memset(url,'x',sizeof(url));
        CHECK(UmiModelBundleUrlAt(&f->plan,1U,url,sizeof(url))==UMI_STATUS_OK);
        CHECK(strcmp(url,"https://huggingface.co/umicom-fixtures/synthetic/resolve/0123456789abcdef0123456789abcdef01234567/unet/model.safetensors")==0);
        size_t needed=strlen(url)+1U;
        memset(url,'x',sizeof(url));
        CHECK(UmiModelBundleUrlAt(&f->plan,1U,url,needed-1U)==UMI_STATUS_CAPACITY_EXCEEDED);
        for(size_t i=0U;i<sizeof(url);++i) CHECK(url[i]=='x');
        CHECK(UmiModelBundleUrlAt(&f->plan,1U,url,needed)==UMI_STATUS_OK);
        CHECK(strlen(url)+1U==needed);
        return 0;
    }
    if(strcmp(name,"query-nulls")==0) {
        CHECK(UmiModelBundlePlanValidate(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelBundleGgufSourceAt(&f->plan,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiModelBundleUrlAt(&f->plan,0U,NULL,0U)==UMI_STATUS_INVALID_ARGUMENT);
        char url[8];memset(url,'x',sizeof(url));
        CHECK(UmiModelBundleUrlAt(&f->plan,0U,url,0U)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiModelBundleUrlAt(&f->plan,SIZE_MAX,url,sizeof(url))==UMI_STATUS_NOT_FOUND);
        for(size_t i=0U;i<sizeof(url);++i) CHECK(url[i]=='x');
        return 0;
    }
    if(strcmp(name,"malformed-storage")==0) {
        f->plan.count=SIZE_MAX;
        CHECK(UmiModelBundlePlanValidate(&f->plan)==UMI_STATUS_CAPACITY_EXCEEDED);
        f->plan.count=3U;memset(f->plan.repository,'x',sizeof(f->plan.repository));
        CHECK(UmiModelBundlePlanValidate(&f->plan)==UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if(strcmp(name,"unused-slots")==0) {
        memset(&f->plan.files[3],0xa5,sizeof(f->plan.files[3]));
        CHECK(UmiModelBundlePlanValidate(&f->plan)==UMI_STATUS_OK);
        return 0;
    }
    fprintf(stderr,"Unknown case: %s\n",name);return 2;
}
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    BundleFixture *fixture=bundle_fixture_create();
    if(fixture==NULL) return 2;
    int result=run_case(fixture,argv[1]);free(fixture);return result;
}
