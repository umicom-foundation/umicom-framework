/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_generator_create_effects.c
 *
 * PURPOSE:
 *   Inject native-boundary outcomes into the real creator to verify effect order, refusal and partial-progress reports without filesystem effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/generator.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);exit(1);}}while(0)
static const char *mode;
static unsigned roots,parents,files;
static UmiStatus ObserveRoot(const char *root,const char *relative,UmiRootedDirectoryCreation *report)
{
    CHECK(root!=NULL&&!strcmp(relative,"project"));++roots;
    CHECK(parents==0U&&files==0U);
    *report=(UmiRootedDirectoryCreation){1U,1,UMI_STATUS_OK};
    if(!strcmp(mode,"root-refused"))*report=(UmiRootedDirectoryCreation){0U,0,UMI_STATUS_ALREADY_EXISTS};
    if(!strcmp(mode,"root-partial"))*report=(UmiRootedDirectoryCreation){1U,0,UMI_STATUS_IO_ERROR};
    return report->status;
}
static UmiStatus ObserveParents(const char *root,const char *relative,UmiRootedDirectoryCreation *report)
{
    CHECK(root!=NULL&&roots==1U);++parents;
    CHECK(!strcmp(relative,parents==1U?"src":"include"));
    CHECK(files+1U==parents);
    *report=(UmiRootedDirectoryCreation){1U,1,UMI_STATUS_OK};
    if(!strcmp(mode,"ensure-refused"))*report=(UmiRootedDirectoryCreation){0U,0,UMI_STATUS_PERMISSION_DENIED};
    if(!strcmp(mode,"ensure-partial"))*report=(UmiRootedDirectoryCreation){1U,0,UMI_STATUS_IO_ERROR};
    return report->status;
}
static UmiStatus ObserveFile(const char *root,const char *relative,const void *bytes,size_t size,UmiRootedFileCreation *report)
{
    CHECK(root!=NULL&&bytes!=NULL&&roots==1U);++files;
    CHECK(parents==files&&!strcmp(relative,files==1U?"src/a.c":"include/b.h"));
    CHECK(size==(files==1U?3U:4U));
    *report=(UmiRootedFileCreation){size,1,1,UMI_STATUS_OK};
    int first=files==1U&&!strncmp(mode,"first-file-",11U);
    int second=files==2U&&!strncmp(mode,"second-file-",12U);
    if(first||second){
        if(strstr(mode,"refused")!=NULL)*report=(UmiRootedFileCreation){0U,0,0,UMI_STATUS_ALREADY_EXISTS};
        else *report=(UmiRootedFileCreation){2U,1,0,UMI_STATUS_IO_ERROR};
    }
    return report->status;
}
#define UmiRootedDirectoryCreate ObserveRoot
#define UmiRootedDirectoryEnsure ObserveParents
#define UmiRootedFileCreate ObserveFile
#define umi_developer_project_generator_create ObserveCreate
#include "../../src/developer_project/generator_create.c"
#undef umi_developer_project_generator_create
#undef UmiRootedFileCreate
#undef UmiRootedDirectoryEnsure
#undef UmiRootedDirectoryCreate

int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    mode=argv[1];
    UmiDeveloperProjectGenerationPlan *plan=calloc(1U,sizeof *plan);
    CHECK(plan!=NULL);umi_developer_project_generation_plan_init(plan);
#ifdef _WIN32
    strcpy(plan->project_root,"C:/qualification/project");
#else
    strcpy(plan->project_root,"/qualification/project");
#endif
    strcpy(plan->template_id,"fixture.effects");plan->file_count=2U;
    strcpy(plan->files[0].relative_path,"src/a.c");strcpy(plan->files[0].content,"abc");
    strcpy(plan->files[1].relative_path,"include/b.h");strcpy(plan->files[1].content,"defg");
    UmiDeveloperProjectGeneratorRequest request={plan,0,0};
    UmiDeveloperProjectGeneratorReport report;
    UmiStatus expected=UMI_STATUS_OK;
    size_t expected_dirs=3U,expected_files=2U,expected_bytes=7U;
    unsigned expected_parents=2U,expected_calls=2U,expected_roots=1U;
    if(!strcmp(mode,"root-refused")||!strcmp(mode,"root-partial")){
        expected=!strcmp(mode,"root-refused")?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_IO_ERROR;
        expected_dirs=!strcmp(mode,"root-refused")?0U:1U;
        expected_files=expected_bytes=expected_parents=expected_calls=0U;
    }else if(!strcmp(mode,"ensure-refused")||!strcmp(mode,"ensure-partial")){
        expected=!strcmp(mode,"ensure-refused")?UMI_STATUS_PERMISSION_DENIED:UMI_STATUS_IO_ERROR;
        expected_dirs=!strcmp(mode,"ensure-refused")?1U:2U;expected_parents=1U;
        expected_files=expected_bytes=expected_calls=0U;
    }else if(!strncmp(mode,"first-file-",11U)||!strncmp(mode,"second-file-",12U)){
        int second=!strncmp(mode,"second-file-",12U),partial=strstr(mode,"partial")!=NULL;
        expected=partial?UMI_STATUS_IO_ERROR:UMI_STATUS_ALREADY_EXISTS;
        expected_dirs=second?3U:2U;expected_calls=expected_parents=second?2U:1U;
        expected_files=(second?1U:0U)+(partial?1U:0U);
        expected_bytes=(second?3U:0U)+(partial?2U:0U);
    }else if(!strcmp(mode,"late-admission")||!strcmp(mode,"overwrite-no-effects")||!strcmp(mode,"null-reset")){
        expected=!strcmp(mode,"late-admission")?UMI_STATUS_PERMISSION_DENIED:UMI_STATUS_INVALID_ARGUMENT;
        expected_dirs=expected_files=expected_bytes=expected_parents=expected_calls=expected_roots=0U;
        if(!strcmp(mode,"late-admission"))strcpy(plan->files[1].relative_path,"../escape");
        if(!strcmp(mode,"overwrite-no-effects"))request.overwrite_existing_files=1;
    }else if(!strcmp(mode,"dry-no-effects")){
        request.dry_run=1;expected_dirs=expected_parents=expected_calls=expected_roots=0U;
    }else if(strcmp(mode,"complete")){free(plan);return 2;}
    memset(&report,0xa5,sizeof report);
    CHECK(ObserveCreate(!strcmp(mode,"null-reset")?NULL:&request,&report)==expected);
    CHECK(report.directories_created==expected_dirs&&report.files_created==expected_files&&report.bytes_written==expected_bytes);
    CHECK(report.files_skipped==0U&&report.dry_run==request.dry_run);
    CHECK(roots==expected_roots&&parents==expected_parents&&files==expected_calls);
    free(plan);return 0;
}
