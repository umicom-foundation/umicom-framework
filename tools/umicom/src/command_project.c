/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tools/umicom/src/command_project.c
 *
 * PURPOSE:
 *   Present the shared project catalogue, owned plans and explicit new-project creation without build or publication side effects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "cli.h"
#include "umicom/developer_project/new_project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int ProjectUsage(void)
{
    puts("Usage: umicom project templates\n"
         "       umicom project plan OPTIONS [--show-files]\n"
         "       umicom project create OPTIONS --apply\n"
         "Options (all required, each once):\n"
         "  --template ID --name DISPLAY_NAME --id APPLICATION_ID\n"
         "  --target TARGET --parent ABSOLUTE_DIRECTORY --folder FOLDER_NAME\n"
         "Plan only renders owned memory; it does not check destination existence.\n"
         "Create requires a new destination and never overwrites existing files.\n"
         "No compiler, test, Git, network or application launch is performed.\n"
         "Partial new files may remain if creation fails. Do not share a destination\n"
         "with a concurrent writer. Existing create/new app commands are unchanged.");
    return ferror(stdout) ? 1 : 0;
}

static UmiStatus ProjectCopy(char *out,size_t capacity,const char *value)
{
    size_t bytes=strlen(value);
    if(bytes>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out,value,bytes+1U);
    return UMI_STATUS_OK;
}

/* Parsing belongs to this command adapter; all recipe, destination, rendering
 * and write policies stay in the same Framework services used by Studio. */
int umi_cli_command_project(int argc,char **argv)
{
    if(argc<0 || (argc!=0&&argv==NULL))return 2;
    for(int i=0;i<argc;++i)if(argv[i]==NULL||argv[i][0]=='\0')return 2;
    if(argc==0 || (argc==1&&(!strcmp(argv[0],"--help")||!strcmp(argv[0],"-h"))))
        return ProjectUsage();
    int listing=!strcmp(argv[0],"templates");
    int planning=!strcmp(argv[0],"plan");
    int creating=!strcmp(argv[0],"create");
    if((!listing&&!planning&&!creating)||(listing&&argc!=1))return 2;

    UmiDeveloperProjectGenerationRequest request;
    umi_developer_project_generation_request_init(&request);
    const char *values[6]={0};
    const char *const options[6]={"--template","--name","--id","--target","--parent","--folder"};
    unsigned seen=0U;
    int apply=0,show_files=0;
    for(int i=1;i<argc;++i) {
        if(!strcmp(argv[i],"--apply")) {
            if(apply||!creating)return 2;
            apply=1;continue;
        }
        if(!strcmp(argv[i],"--show-files")) {
            if(show_files||!planning)return 2;
            show_files=1;continue;
        }
        size_t option=0U;
        for(;option<6U;++option)if(!strcmp(argv[i],options[option]))break;
        if(option==6U || (seen&(1U<<option))!=0U || i+1>=argc)return 2;
        seen|=1U<<option;
        values[option]=argv[++i];
    }
    if(!listing && (seen!=63U || (creating&&!apply))) {
        fputs("Project request refused: provide every identity/destination option; create also requires --apply.\n",stderr);
        return 2;
    }

    UmiStatus status=UMI_STATUS_OK;
    if(!listing) {
        status=ProjectCopy(request.template_id,sizeof request.template_id,values[0]);
        if(status==UMI_STATUS_OK)status=ProjectCopy(request.application_name,sizeof request.application_name,values[1]);
        if(status==UMI_STATUS_OK)status=ProjectCopy(request.application_id,sizeof request.application_id,values[2]);
        if(status==UMI_STATUS_OK)status=ProjectCopy(request.target_name,sizeof request.target_name,values[3]);
        if(status==UMI_STATUS_OK)status=ProjectCopy(request.repository_name,sizeof request.repository_name,values[5]);
        if(status==UMI_STATUS_OK)status=UmiDeveloperProjectChooseDestination(
            values[4],values[5],request.project_root,sizeof request.project_root);
        if(status==UMI_STATUS_OK)status=umi_developer_project_generation_request_validate(&request,NULL,0U);
        if(status!=UMI_STATUS_OK) {
            fprintf(stderr,"Project options refused: %s\n",umi_status_text(status));
            return 2;
        }
        request.dry_run=planning;
        request.overwrite_existing_files=0;
    }

    UmiDeveloperProjectService *service=NULL;
    status=umi_developer_project_service_create(&service);
    if(status!=UMI_STATUS_OK) {
        fprintf(stderr,"Project service unavailable: %s\n",umi_status_text(status));
        return 1;
    }
    if(listing) {
        UmiDeveloperProjectTemplateRegistry *registry=umi_developer_project_service_templates(service);
        size_t count=umi_developer_project_template_registry_count(registry);
        for(size_t i=0U;i<count;++i) {
            const UmiDeveloperProjectTemplate *recipe=umi_developer_project_template_registry_at(registry,i);
            if(recipe==NULL){status=UMI_STATUS_INVALID_STATE;break;}
            printf("%s | %s | %zu files\n",recipe->template_id,recipe->title,recipe->file_count);
        }
        puts("Catalogue only. Installed toolchains, SDKs and generated applications are unverified.");
    } else if(planning) {
        UmiDeveloperProjectGenerationPlan *plan=calloc(1U,sizeof *plan);
        /* Framework declares OUT_OF_MEMORY. Preserve the earlier spelling below
         * for integration review; the active path keeps the same failure behavior. */
#if 0
        if(plan==NULL)status=UMI_STATUS_NO_MEMORY;
#else
        if(plan==NULL)status=UMI_STATUS_OUT_OF_MEMORY;
#endif
        else {
            status=umi_developer_project_service_plan_generation(service,&request,plan);
            if(status==UMI_STATUS_OK) {
                printf("Project plan: %s\nDestination: %s\nFiles: %zu | Text bytes: %zu\n",
                    plan->template_id,plan->project_root,plan->file_count,plan->total_content_bytes);
                for(size_t i=0U;i<plan->file_count;++i) {
                    printf("File: %s | %zu bytes\n",plan->files[i].relative_path,strlen(plan->files[i].content));
                    if(show_files) { puts("--- begin file ---");fputs(plan->files[i].content,stdout);puts("\n--- end file ---"); }
                }
                puts("Plan only. No files created; destination availability and generated builds remain unverified.");
            }
            free(plan);
        }
    } else {
        typedef struct ProjectOutputs {
            UmiDeveloperProjectModel model;
            UmiBuildProfile profile;
        } ProjectOutputs;
        ProjectOutputs *out=calloc(1U,sizeof *out);
        UmiDeveloperProjectGeneratorReport report={0};
        /* Framework declares OUT_OF_MEMORY. Preserve the earlier spelling below
         * for integration review; the active path keeps the same failure behavior. */
#if 0
        if(out==NULL)status=UMI_STATUS_NO_MEMORY;
#else
        if(out==NULL)status=UMI_STATUS_OUT_OF_MEMORY;
#endif
        else {
            status=UmiDeveloperProjectCreateNew(service,&request,&report,&out->model,&out->profile);
            /* A refusal may follow some writes. Keep that observation instead
             * of implying rollback or retrying automatically into partial work. */
            /* Preserve the older completion wording for review. A created file
             * may be incomplete after a failed write, flush or close. */
#if 0
            printf("Creation attempt: %zu files written | %zu skipped | %zu text bytes\n",
                report.files_created,report.files_skipped,report.bytes_written);
#endif
            /* Keep the previous adapter-owned wording for comparison. Shared
             * outcome text now gives CLI and native forms the same semantics. */
#if 0
            printf("Creation attempt: %zu directories created | %zu files created | %zu skipped | %zu bytes accepted\n",
                report.directories_created,report.files_created,report.files_skipped,report.bytes_written);
            if(status!=UMI_STATUS_OK && (report.directories_created!=0U||report.files_created!=0U))
                puts("Partial work retained. Counts do not establish complete files; inspect the failure before choosing a new destination.");
#endif
            char description[1024];
            if(UmiDeveloperProjectCreationDescribe(status,&report,description,sizeof description)==UMI_STATUS_OK)
                puts(description);
            else fputs("Creation progress could not be described; retain the status and inspect the destination.\n",stderr);
            if(status==UMI_STATUS_OK) {
                printf("Created project: %s\nDestination: %s\n",request.application_name,request.project_root);
                puts("Source created only. No Git repository, build, test, install or application launch was performed.");
            }
            free(out);
        }
    }
    umi_developer_project_service_destroy(service);
    if(status!=UMI_STATUS_OK)fprintf(stderr,"Project operation refused: %s\n",umi_status_text(status));
    if(ferror(stdout)||ferror(stderr))return 1;
    return status==UMI_STATUS_OK?0:1;
}
