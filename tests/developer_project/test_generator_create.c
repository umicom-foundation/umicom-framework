/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_generator_create.c
 *
 * PURPOSE:
 *   Qualify fresh-project creation, retained destinations and no-follow parent boundaries with isolated native fixtures.
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
#include "umicom/developer_project/generator.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"

static int LinkDirectory(const char *alias,const char *target)
{
#ifdef _WIN32
    return FixtureSymbolicLink(alias,target,1);
#else
    CHECK(symlink(target,alias)==0);return 1;
#endif
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1];
    char root[UMI_PATH_CAPACITY],destination[UMI_PATH_CAPACITY],file[UMI_PATH_CAPACITY];
    FixtureDirectory(root);FixturePath(destination,root,"project");
    UmiDeveloperProjectGenerationPlan *plan=calloc(1U,sizeof *plan);
    CHECK(plan!=NULL);umi_developer_project_generation_plan_init(plan);
    CHECK(strlen(destination)<sizeof plan->project_root);
    strcpy(plan->project_root,destination);strcpy(plan->template_id,"fixture.create");
    plan->file_count=2U;
    strcpy(plan->files[0].relative_path,"src/main.c");strcpy(plan->files[0].content,"first\n");
    strcpy(plan->files[1].relative_path,"README.md");strcpy(plan->files[1].content,"second\n");
    UmiDeveloperProjectGeneratorRequest request={plan,0,0};
    UmiDeveloperProjectGeneratorReport report;
    UmiRootedDirectoryCreation directory;UmiRootedFileCreation creation;
    UmiStatus expected=UMI_STATUS_OK;
    int effects=1,existing=0,link=0;
    if(!strcmp(mode,"repeat")){}
    else if(!strcmp(mode,"complete")){}
    else if(!strcmp(mode,"empty-content"))plan->files[1].content[0]='\0';
    else if(!strcmp(mode,"separators"))strcpy(plan->files[0].relative_path,"src\\main.c");
    else if(!strcmp(mode,"unicode"))strcpy(plan->files[0].relative_path,"caf\xc3\xa9/\xe6\x96\x87.c");
    else if(!strcmp(mode,"existing-directory")||!strcmp(mode,"dry-existing")){
        CHECK(UmiRootedDirectoryEnsure(root,"project",&directory)==UMI_STATUS_OK);
        CHECK(UmiRootedFileCreate(root,"project/keep.txt","keep",4U,&creation)==UMI_STATUS_OK);
        effects=0;existing=1;
        if(!strcmp(mode,"dry-existing"))request.dry_run=1;
        else expected=UMI_STATUS_ALREADY_EXISTS;
    }else if(!strcmp(mode,"existing-file")){
        CHECK(UmiRootedFileCreate(root,"project","keep",4U,&creation)==UMI_STATUS_OK);
        expected=UMI_STATUS_ALREADY_EXISTS;effects=0;existing=2;
    }else if(!strcmp(mode,"missing-parent")||!strcmp(mode,"dry-missing-parent")){
        FixturePath(destination,root,"absent/project");strcpy(plan->project_root,destination);
        effects=0;
        if(!strcmp(mode,"dry-missing-parent"))request.dry_run=1;
        else expected=UMI_STATUS_NOT_FOUND;
    }else if(!strcmp(mode,"parent-link")||!strcmp(mode,"root-link")){
        CHECK(UmiRootedDirectoryEnsure(root,"target",&directory)==UMI_STATUS_OK);
        char target[UMI_PATH_CAPACITY];FixturePath(target,root,"target");
        FixturePath(file,root,"alias");
        if(!LinkDirectory(file,target)){free(plan);fputs("SKIP: directory link fixture unavailable.\n",stderr);return 77;}
        FixturePath(destination,root,!strcmp(mode,"parent-link")?"alias/project":"alias");
        strcpy(plan->project_root,destination);effects=0;link=1;
        expected=!strcmp(mode,"parent-link")?UMI_STATUS_PERMISSION_DENIED:UMI_STATUS_ALREADY_EXISTS;
    }else if(!strcmp(mode,"invalid-late")){
        strcpy(plan->files[1].relative_path,"../outside");
        expected=UMI_STATUS_PERMISSION_DENIED;effects=0;
    }else if(!strcmp(mode,"overwrite")){
        request.overwrite_existing_files=1;expected=UMI_STATUS_INVALID_ARGUMENT;effects=0;
    }else if(!strcmp(mode,"null-request")){
        memset(&report,0xa5,sizeof report);
        CHECK(umi_developer_project_generator_create(NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(report.directories_created==0U&&report.files_created==0U&&report.bytes_written==0U&&report.files_skipped==0U);
        CHECK(!umi_fs_exists(destination));free(plan);return 0;
    }else{free(plan);return 2;}
    memset(&report,0xa5,sizeof report);
    CHECK(umi_developer_project_generator_create(&request,&report)==expected);
    CHECK(report.files_skipped==0U&&report.dry_run==request.dry_run);
    if(effects){
        CHECK(report.directories_created==2U&&report.files_created==2U);
        CHECK(report.bytes_written==strlen(plan->files[0].content)+strlen(plan->files[1].content));
        for(size_t i=0U;i<plan->file_count;++i){
            unsigned char *bytes=NULL;size_t count=0U,length=strlen(plan->files[i].content);
            CHECK(UmiRootedFileRead(destination,plan->files[i].relative_path,64U,&bytes,&count)==UMI_STATUS_OK);
            CHECK(count==length&&memcmp(bytes,plan->files[i].content,length)==0);UmiRootedFileFree(bytes);
        }
        if(!strcmp(mode,"repeat")){
            CHECK(umi_developer_project_generator_create(&request,&report)==UMI_STATUS_ALREADY_EXISTS);
            CHECK(report.files_created==0U&&report.bytes_written==0U&&report.directories_created==0U);
            unsigned char *bytes=NULL;size_t count=0U;
            CHECK(UmiRootedFileRead(destination,"src/main.c",64U,&bytes,&count)==UMI_STATUS_OK);
            CHECK(count==6U&&memcmp(bytes,"first\n",6U)==0);UmiRootedFileFree(bytes);
        }
    }else{
        CHECK(report.directories_created==0U);
        if(request.dry_run)CHECK(report.files_created==2U&&report.bytes_written==13U);
        else CHECK(report.files_created==0U&&report.bytes_written==0U);
        if(existing){
            FixturePath(file,root,existing==1?"project/keep.txt":"project");
            size_t count=0U;unsigned char *bytes=FixtureRead(file,&count);
            CHECK(count==4U&&memcmp(bytes,"keep",4U)==0);free(bytes);
            if(existing==1){FixturePath(file,root,"project/README.md");CHECK(!umi_fs_exists(file));}
        }else if(link){FixturePath(file,root,"target/README.md");CHECK(!umi_fs_exists(file));FixturePath(file,root,"target/project");CHECK(!umi_fs_exists(file));}
        else CHECK(!umi_fs_exists(destination));
    }
    free(plan);return 0;
}
