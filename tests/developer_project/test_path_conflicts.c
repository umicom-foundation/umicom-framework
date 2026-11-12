/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_path_conflicts.c
 *
 * PURPOSE:
 *   Refuse ambiguous project paths before filesystem probes and reset each generator attempt report.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/generator.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static size_t probes, effects;
static int ObserveDirectory(const char *path) { (void)path; ++probes; return 1; }
static int ObserveExists(const char *path) { (void)path; ++probes; return 0; }
static UmiStatus ObserveMkdir(const char *path) { (void)path; ++effects; return UMI_STATUS_OK; }
static UmiStatus ObserveWrite(const char *path,const char *text)
{ (void)path; (void)text; ++effects; return UMI_STATUS_OK; }

/* Compile the actual generator with only its filesystem effect/probe boundary
 * replaced. Plan validation and path joining remain real shared services.
 * No owner qualification directory or user file is touched by these cases. */
#define umi_fs_is_directory ObserveDirectory
#define umi_fs_exists ObserveExists
#define umi_fs_make_directories ObserveMkdir
#define umi_fs_write_text ObserveWrite
#define umi_developer_project_generator_apply ObserveGenerator
#include "../../src/developer_project/generator.c"
#undef umi_developer_project_generator_apply
#undef umi_fs_write_text
#undef umi_fs_make_directories
#undef umi_fs_exists
#undef umi_fs_is_directory

#define CHECK(c) do { if (!(c)) { fprintf(stderr,"%d: %s\n",__LINE__,#c); goto failed; } } while (0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *name=argv[1];
    UmiDeveloperProjectGenerationPlan *plan=calloc(1U,sizeof *plan);
    UmiDeveloperProjectGenerationPlan *before=malloc(sizeof *before);
    CHECK(plan!=NULL&&before!=NULL);
    strcpy(plan->project_root,"C:/qualification/project");
    strcpy(plan->template_id,"fixture.portable-paths");
    strcpy(plan->files[0].relative_path,"src/main.c");
    strcpy(plan->files[1].relative_path,"README.md");
    strcpy(plan->files[0].content,"first");
    strcpy(plan->files[1].content,"second");
    plan->file_count=2U;plan->revision=1U;plan->total_content_bytes=11U;
    UmiDeveloperProjectGeneratorRequest request={plan,1,0};
    const UmiDeveloperProjectGeneratorRequest *input=&request;
    UmiStatus expected=UMI_STATUS_OK;
    if(!strcmp(name,"valid")) {}
    else if(!strcmp(name,"ascii-case")) {strcpy(plan->files[1].relative_path,"SRC/Main.C");expected=UMI_STATUS_ALREADY_EXISTS;}
    else if(!strcmp(name,"separators")) {strcpy(plan->files[1].relative_path,"src\\main.c");expected=UMI_STATUS_ALREADY_EXISTS;}
    else if(!strcmp(name,"first-parent")) {strcpy(plan->files[0].relative_path,"src");strcpy(plan->files[1].relative_path,"SRC/main.c");expected=UMI_STATUS_ALREADY_EXISTS;}
    else if(!strcmp(name,"second-parent")) {strcpy(plan->files[1].relative_path,"SRC");expected=UMI_STATUS_ALREADY_EXISTS;}
    else if(!strcmp(name,"siblings")) strcpy(plan->files[1].relative_path,"src/main.cpp");
    else if(!strcmp(name,"later-traversal")) {strcpy(plan->files[1].relative_path,"../outside");expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(name,"git")) {strcpy(plan->files[1].relative_path,".git/config");expected=UMI_STATUS_PERMISSION_DENIED;}
    else if(!strcmp(name,"reserved")) {strcpy(plan->files[1].relative_path,"CON.txt");expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(name,"duplicate-before-invalid")) {
        strcpy(plan->files[1].relative_path,"src/main.c");strcpy(plan->files[2].relative_path,"../outside");
        plan->file_count=3U;expected=UMI_STATUS_PERMISSION_DENIED;
    } else if(!strcmp(name,"trailing-dot")) {strcpy(plan->files[1].relative_path,"name.");expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(name,"trailing-space")) {strcpy(plan->files[1].relative_path,"name ");expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(name,"empty-component")) {strcpy(plan->files[1].relative_path,"src//other.c");expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(name,"late-join-capacity")) {
        memset(plan->project_root,'r',1900U);memcpy(plan->project_root,"C:/",3U);plan->project_root[1900]='\0';
        memset(plan->files[1].relative_path,'f',200U);plan->files[1].relative_path[200]='\0';
        expected=UMI_STATUS_CAPACITY_EXCEEDED;
    } else if(!strcmp(name,"null-request")) {input=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(name,"missing-plan")) {request.plan=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(name,"unterminated")) {memset(plan->files[1].relative_path,'x',sizeof plan->files[1].relative_path);expected=UMI_STATUS_INVALID_ARGUMENT;}
    else {free(before);free(plan);return 2;}
    memcpy(before,plan,sizeof *before);
    UmiDeveloperProjectGeneratorReport report;
    memset(&report,0xa5,sizeof report);
    CHECK(ObserveGenerator(input,&report)==expected);
    CHECK(memcmp(before,plan,sizeof *plan)==0);
    CHECK(effects==0U);
    if(expected==UMI_STATUS_OK) {
        CHECK(probes>0U&&report.files_created==2U&&report.bytes_written==11U&&report.dry_run==1);
    } else {
        CHECK(probes==0U&&report.directories_created==0U&&report.files_created==0U);
        CHECK(report.files_skipped==0U&&report.bytes_written==0U);
        CHECK(report.dry_run==((input!=NULL&&input->plan!=NULL)?1:0));
    }
    free(before);free(plan);return 0;
failed:
    free(before);free(plan);return 1;
}
