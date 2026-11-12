/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_new_project_preflight.c
 *
 * PURPOSE:
 *   Verify profile and registry admission precede writer dispatch and failed writes do not publish model/profile outputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);exit(1);}}while(0)
static const char *mode;
static unsigned profiles,writes;
static UmiStatus ObserveProfile(const UmiDeveloperProjectModel *model,UmiBuildProfile *profile)
{
    ++profiles;CHECK(writes==0U);
    if(!strcmp(mode,"profile-refusal"))return UMI_STATUS_INVALID_ARGUMENT;
    return UmiBuildProfileFromProject(model,profile);
}
static UmiStatus ObserveWriter(const UmiDeveloperProjectGeneratorRequest *request,UmiDeveloperProjectGeneratorReport *report)
{
    ++writes;CHECK(profiles==1U);
    *report=(UmiDeveloperProjectGeneratorReport){0};
    report->dry_run=request->dry_run!=0;
    if(!strcmp(mode,"writer-refusal")){
        report->directories_created=2U;report->files_created=1U;report->bytes_written=2U;
        return UMI_STATUS_IO_ERROR;
    }
    report->files_created=request->plan->file_count;
    report->bytes_written=request->plan->total_content_bytes;
    return UMI_STATUS_OK;
}
#define UmiBuildProfileFromProject ObserveProfile
#define umi_developer_project_generator_create ObserveWriter
#include "../../src/developer_project/service.c"
#undef umi_developer_project_generator_create
#undef UmiBuildProfileFromProject

int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    mode=argv[1];
    UmiDeveloperProjectService *service=NULL;
    CHECK(umi_developer_project_service_create(&service)==UMI_STATUS_OK);
    UmiDeveloperProjectModel *model=malloc(sizeof *model),*saved=malloc(sizeof *saved);
    UmiBuildProfile *profile=malloc(sizeof *profile),*saved_profile=malloc(sizeof *saved_profile);
    CHECK(model!=NULL&&saved!=NULL&&profile!=NULL&&saved_profile!=NULL);
    UmiDeveloperProjectGenerationRequest request;umi_developer_project_generation_request_init(&request);
    strcpy(request.template_id,"developer.template.c23-console");
    strcpy(request.application_name,"Umicom Ledger");strcpy(request.application_id,"org.umicom.ledger");
    strcpy(request.repository_name,"umicom-ledger");strcpy(request.target_name,"umicom_ledger");
#ifdef _WIN32
    strcpy(request.project_root,"C:/qualification/project");
#else
    strcpy(request.project_root,"/qualification/project");
#endif
    request.dry_run=0;
    UmiStatus expected=UMI_STATUS_OK;unsigned expected_profiles=1U,expected_writes=1U;
    if(!strcmp(mode,"profile-refusal")){expected=UMI_STATUS_INVALID_ARGUMENT;expected_writes=0U;}
    else if(!strcmp(mode,"writer-refusal"))expected=UMI_STATUS_IO_ERROR;
    else if(!strcmp(mode,"dry-run"))request.dry_run=1;
    else if(!strcmp(mode,"relative-root")||!strcmp(mode,"overwrite")||!strcmp(mode,"invalid-root")){
        request.dry_run=1;expected=UMI_STATUS_INVALID_ARGUMENT;expected_profiles=expected_writes=0U;
        if(!strcmp(mode,"relative-root"))strcpy(request.project_root,"relative/project");
        else if(!strcmp(mode,"overwrite"))request.overwrite_existing_files=1;
        else strcat(request.project_root,"/../other");
    }
    else if(!strcmp(mode,"revision-exhausted")){
        service->revision=UINT64_MAX;expected=UMI_STATUS_CAPACITY_EXCEEDED;expected_profiles=expected_writes=0U;
    }else if(!strcmp(mode,"full-registry")||!strcmp(mode,"full-registry-upsert")){
        for(size_t index=0U;index<UMI_DEVELOPER_PROJECT_MODEL_CAPACITY;++index){
            char id[64];snprintf(id,sizeof id,"fixture.%zu",index);
            umi_developer_project_model_init(model,id,"Existing project");
            strcpy(model->root,request.project_root);strcpy(model->primary_language_id,"developer.language.c23");
            CHECK(umi_developer_project_model_registry_upsert(service->models,model)==UMI_STATUS_OK);
        }
        if(!strcmp(mode,"full-registry")){expected=UMI_STATUS_CAPACITY_EXCEEDED;expected_writes=0U;}
        else strcpy(request.application_id,"fixture.0");
    }else if(strcmp(mode,"complete"))return 2;
    memset(model,0xa5,sizeof *model);memcpy(saved,model,sizeof *model);
    memset(profile,0xa5,sizeof *profile);memcpy(saved_profile,profile,sizeof *profile);
    UmiDeveloperProjectServiceSnapshot before,after;
    CHECK(umi_developer_project_service_snapshot(service,&before)==UMI_STATUS_OK);
    UmiDeveloperProjectGeneratorReport report;memset(&report,0xa5,sizeof report);
    CHECK(umi_developer_project_service_generate_new(service,&request,&report,model,profile)==expected);
    CHECK(profiles==expected_profiles&&writes==expected_writes);
    CHECK(umi_developer_project_service_snapshot(service,&after)==UMI_STATUS_OK);
    if(expected!=UMI_STATUS_OK){
        CHECK(memcmp(model,saved,sizeof *model)==0&&memcmp(profile,saved_profile,sizeof *profile)==0);
        CHECK(after.project_count==before.project_count&&after.revision==before.revision);
        if(!strcmp(mode,"writer-refusal"))CHECK(report.files_created==1U&&report.bytes_written==2U&&report.directories_created==2U);
        else CHECK(report.files_created==0U&&report.bytes_written==0U&&report.directories_created==0U);
    }else{
        CHECK(!strcmp(model->project_id,request.application_id));
        CHECK(umi_build_profile_validate(profile,NULL,0U)==UMI_STATUS_OK);
        CHECK(after.revision==before.revision+(request.dry_run?0U:1U));
        size_t added=request.dry_run||!strcmp(mode,"full-registry-upsert")?0U:1U;
        CHECK(after.project_count==before.project_count+added);
    }
    free(model);free(saved);free(profile);free(saved_profile);
    umi_developer_project_service_destroy(service);return 0;
}
