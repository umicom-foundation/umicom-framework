/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_new_project.c
 *
 * PURPOSE:
 *   Qualify new-project service composition, immutable refusal outputs and registry publication with retained native fixtures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/developer_project/new_project.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"

int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1];
    char root[UMI_PATH_CAPACITY],destination[UMI_PATH_CAPACITY],full[UMI_PATH_CAPACITY];
    FixtureDirectory(root);FixturePath(destination,root,"project");
    UmiDeveloperProjectService *service=NULL;
    CHECK(umi_developer_project_service_create(&service)==UMI_STATUS_OK);
    UmiDeveloperProjectModel *model=malloc(sizeof *model),*model_before=malloc(sizeof *model_before);
    UmiBuildProfile *profile=malloc(sizeof *profile),*profile_before=malloc(sizeof *profile_before);
    CHECK(model!=NULL&&model_before!=NULL&&profile!=NULL&&profile_before!=NULL);
    memset(model,0xa5,sizeof *model);memcpy(model_before,model,sizeof *model);
    memset(profile,0xa5,sizeof *profile);memcpy(profile_before,profile,sizeof *profile);
    UmiDeveloperProjectGenerationRequest request;
    umi_developer_project_generation_request_init(&request);
    strcpy(request.template_id,"developer.template.c23-console");
    strcpy(request.application_name,"Umicom Ledger");strcpy(request.application_id,"org.umicom.ledger");
    strcpy(request.repository_name,"umicom-ledger");strcpy(request.target_name,"umicom_ledger");
    strcpy(request.project_root,destination);request.dry_run=0;
    UmiRootedDirectoryCreation directory;UmiRootedFileCreation creation;
    UmiStatus expected=UMI_STATUS_OK;int applied=1;
    if(!strcmp(mode,"complete")||!strcmp(mode,"repeat")){}
    else if(!strcmp(mode,"dry-run")){request.dry_run=1;applied=0;}
    else if(!strcmp(mode,"existing-directory")||!strcmp(mode,"dry-existing")){
        CHECK(UmiRootedDirectoryEnsure(root,"project",&directory)==UMI_STATUS_OK);
        CHECK(UmiRootedFileCreate(root,"project/keep.txt","keep",4U,&creation)==UMI_STATUS_OK);
        if(!strcmp(mode,"dry-existing"))request.dry_run=1;
        expected=UMI_STATUS_ALREADY_EXISTS;applied=0;
    }else if(!strcmp(mode,"existing-file")){
        CHECK(UmiRootedFileCreate(root,"project","keep",4U,&creation)==UMI_STATUS_OK);
        expected=UMI_STATUS_ALREADY_EXISTS;applied=0;
    }else if(!strcmp(mode,"missing-parent")){
        FixturePath(destination,root,"absent/project");strcpy(request.project_root,destination);
        expected=UMI_STATUS_NOT_FOUND;applied=0;
    }else if(!strcmp(mode,"parent-link")){
        CHECK(UmiRootedDirectoryEnsure(root,"target",&directory)==UMI_STATUS_OK);
        FixturePath(full,root,"alias");char target[UMI_PATH_CAPACITY];FixturePath(target,root,"target");
#ifdef _WIN32
        if(!FixtureSymbolicLink(full,target,1)){
            free(model);free(model_before);free(profile);free(profile_before);
            umi_developer_project_service_destroy(service);
            fputs("SKIP: directory link fixture unavailable.\n",stderr);return 77;
        }
#else
        CHECK(symlink(target,full)==0);
#endif
        FixturePath(destination,root,"alias/project");strcpy(request.project_root,destination);
        expected=UMI_STATUS_PERMISSION_DENIED;applied=0;
    }else if(!strcmp(mode,"invalid-request")){
        request.application_id[0]='\0';expected=UMI_STATUS_INVALID_ARGUMENT;applied=0;
    }else if(!strcmp(mode,"unknown-template")){
        strcpy(request.template_id,"developer.template.missing");expected=UMI_STATUS_NOT_FOUND;applied=0;
    }else if(!strcmp(mode,"overwrite")){
        request.overwrite_existing_files=1;expected=UMI_STATUS_INVALID_ARGUMENT;applied=0;
    }else if(!strcmp(mode,"null-output")||!strcmp(mode,"null-service")){
        expected=UMI_STATUS_INVALID_ARGUMENT;applied=0;
    }else return 2;
    UmiDeveloperProjectServiceSnapshot before,after;
    CHECK(umi_developer_project_service_snapshot(service,&before)==UMI_STATUS_OK);
    UmiDeveloperProjectGeneratorReport report;memset(&report,0xa5,sizeof report);
    CHECK(UmiDeveloperProjectCreateNew(!strcmp(mode,"null-service")?NULL:service,&request,&report,
        !strcmp(mode,"null-output")?NULL:model,profile)==expected);
    CHECK(umi_developer_project_service_snapshot(service,&after)==UMI_STATUS_OK);
    CHECK(after.project_count==before.project_count+(applied?1U:0U));
    CHECK(after.revision==before.revision+(applied?1U:0U));
    if(expected==UMI_STATUS_OK){
        CHECK(!strcmp(model->project_id,"org.umicom.ledger")&&!strcmp(model->root,destination));
        CHECK(umi_build_profile_validate(profile,NULL,0U)==UMI_STATUS_OK);
        CHECK(!strcmp(profile->source_directory,destination)&&report.files_created>0U&&report.files_skipped==0U);
        if(applied){FixturePath(full,destination,"src/main.c");CHECK(umi_fs_exists(full));}
        else CHECK(!umi_fs_exists(destination)&&report.directories_created==0U);
    }else{
        CHECK(memcmp(model,model_before,sizeof *model)==0&&memcmp(profile,profile_before,sizeof *profile)==0);
        CHECK(report.files_created==0U&&report.bytes_written==0U&&report.directories_created==0U&&report.files_skipped==0U);
        if(!strcmp(mode,"existing-file")||!strcmp(mode,"existing-directory")||!strcmp(mode,"dry-existing")){
            FixturePath(full,root,!strcmp(mode,"existing-file")?"project":"project/keep.txt");
            size_t count=0U;unsigned char *bytes=FixtureRead(full,&count);
            CHECK(count==4U&&memcmp(bytes,"keep",4U)==0);free(bytes);
        }else CHECK(!umi_fs_exists(destination));
    }
    if(!strcmp(mode,"repeat")){
        memcpy(model_before,model,sizeof *model);memcpy(profile_before,profile,sizeof *profile);before=after;
        CHECK(UmiDeveloperProjectCreateNew(service,&request,&report,model,profile)==UMI_STATUS_ALREADY_EXISTS);
        CHECK(umi_developer_project_service_snapshot(service,&after)==UMI_STATUS_OK);
        CHECK(after.project_count==before.project_count&&after.revision==before.revision);
        CHECK(memcmp(model,model_before,sizeof *model)==0&&memcmp(profile,profile_before,sizeof *profile)==0);
        CHECK(report.files_created==0U&&report.directories_created==0U);
    }
    free(model);free(model_before);free(profile);free(profile_before);
    umi_developer_project_service_destroy(service);return 0;
}
