/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/generator_create.c
 *
 * PURPOSE:
 *   Create a fresh project with exclusive native names, complete lexical admission and honest partial-progress reporting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/generator.h"
#include "umicom/platform/path.h"
#include "umicom/platform/rooted_files.h"
#include <string.h>

UmiStatus umi_developer_project_generator_create(
    const UmiDeveloperProjectGeneratorRequest *request,
    UmiDeveloperProjectGeneratorReport *out_report)
{
    UmiDeveloperProjectGeneratorReport local_report;
    UmiDeveloperProjectGeneratorReport *report=out_report!=NULL?out_report:&local_report;
    memset(report,0,sizeof *report);
    if(request==NULL||request->plan==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    report->dry_run=request->dry_run!=0;
    if(request->overwrite_existing_files)return UMI_STATUS_INVALID_ARGUMENT;
    const UmiDeveloperProjectGenerationPlan *plan=request->plan;
    UmiStatus status=umi_developer_project_generation_plan_validate(plan);
    if(status!=UMI_STATUS_OK)return status;

    /* Reuse native name admission before normalization can hide an invalid
     * component. The selected parent already exists; this operation creates
     * only the requested project and its own descendants. */
    status=UmiRootedFileValidatePath(plan->project_root,plan->files[0].relative_path);
    char root[UMI_PATH_CAPACITY],parent[UMI_PATH_CAPACITY],leaf[UMI_PATH_CAPACITY];
    if(status==UMI_STATUS_OK)status=umi_path_normalise(plan->project_root,root,sizeof root);
    if(status==UMI_STATUS_OK)status=umi_path_parent(root,parent,sizeof parent);
    if(status==UMI_STATUS_OK)status=umi_path_basename(root,leaf,sizeof leaf);
    if(status==UMI_STATUS_OK)status=UmiRootedFileValidatePath(parent,leaf);
    if(status!=UMI_STATUS_OK)return status;
    size_t predicted_bytes=0U;
    for(size_t index=0U;index<plan->file_count;++index){
        char destination[UMI_PATH_CAPACITY];
        status=UmiRootedFileValidatePath(root,plan->files[index].relative_path);
        if(status==UMI_STATUS_OK)status=umi_path_join(root,
            plan->files[index].relative_path,destination,sizeof destination);
        if(status!=UMI_STATUS_OK)return status;
        predicted_bytes+=strlen(plan->files[index].content);
    }
    if(request->dry_run){
        /* Preview counts describe intended files/bytes, not observed effects.
         * No directory count is invented without inspecting the filesystem. */
        report->files_created=plan->file_count;
        report->bytes_written=predicted_bytes;
        return UMI_STATUS_OK;
    }

    UmiRootedDirectoryCreation directory;
    status=UmiRootedDirectoryCreate(parent,leaf,&directory);
    report->directories_created+=directory.directories_created;
    if(status!=UMI_STATUS_OK)return status;
    for(size_t index=0U;index<plan->file_count;++index){
        const UmiDeveloperProjectGeneratedFile *file=&plan->files[index];
        char relative[UMI_PATH_CAPACITY],relative_parent[UMI_PATH_CAPACITY];
        status=umi_path_normalise(file->relative_path,relative,sizeof relative);
        if(status==UMI_STATUS_OK)status=umi_path_parent(relative,relative_parent,sizeof relative_parent);
        if(status!=UMI_STATUS_OK)return status;
        if(strcmp(relative_parent,".")!=0){
            status=UmiRootedDirectoryEnsure(root,relative_parent,&directory);
            report->directories_created+=directory.directories_created;
            if(status!=UMI_STATUS_OK)return status;
        }
        UmiRootedFileCreation creation;
        status=UmiRootedFileCreate(root,relative,file->content,strlen(file->content),&creation);
        /* A failed flush/close can still leave a created file. Preserve those
         * effects in the report; never delete partial work or retry overwrite. */
        report->files_created+=creation.created!=0;
        report->bytes_written+=creation.bytes_written;
        if(status!=UMI_STATUS_OK)return status;
    }
    return UMI_STATUS_OK;
}
