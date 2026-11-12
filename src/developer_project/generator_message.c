/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/generator_message.c
 *
 * PURPOSE:
 *   Describe checked new-project results consistently without mistaking partial native effects for completed files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/generator.h"
#include <stdio.h>
#include <string.h>

UmiStatus UmiDeveloperProjectCreationDescribe(UmiStatus result,
    const UmiDeveloperProjectGeneratorReport *report,char *out,size_t capacity)
{
    if(out==NULL||capacity==0U)return UMI_STATUS_INVALID_ARGUMENT;
    if(report!=NULL&&(report->files_skipped!=0U||
        (report->dry_run!=0&&report->dry_run!=1)||
        (report->dry_run&&report->directories_created!=0U)||
        (report->files_created==0U&&report->bytes_written!=0U)))
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *status=umi_status_text(result);
    if(status==NULL)status="Unknown status";
    /* Format into an owned candidate: a short output buffer must preserve the
     * caller's previous useful message instead of publishing truncated advice. */
    char candidate[1024];int length;
    if(report==NULL)
        length=snprintf(candidate,sizeof candidate,
            "Creation result: %s. Progress is unavailable; inspect the destination before retrying.",status);
    else if(report->dry_run)
        length=result==UMI_STATUS_OK
            ?snprintf(candidate,sizeof candidate,
                "Preview prepared: %zu planned files, %zu planned bytes. This is not a destination reservation or a completed build.",
                report->files_created,report->bytes_written)
            :snprintf(candidate,sizeof candidate,
                "Preview refused: %s. Predicted counts are not completed filesystem work.",status);
    else if(result==UMI_STATUS_OK)
        length=snprintf(candidate,sizeof candidate,
            "Project source created: %zu directories, %zu files, %zu bytes accepted. Build, test, install and launch are separate steps.",
            report->directories_created,report->files_created,report->bytes_written);
    else if(report->directories_created!=0U||report->files_created!=0U)
        length=snprintf(candidate,sizeof candidate,
            "Creation stopped: %s. %zu directories and %zu files were created; %zu bytes were accepted. Partial work was retained and files may be incomplete. Inspect the destination before choosing another new folder.",
            status,report->directories_created,report->files_created,report->bytes_written);
    else if(result==UMI_STATUS_ALREADY_EXISTS)
        length=snprintf(candidate,sizeof candidate,
            "That destination name already exists. Choose a new project folder; existing content was not replaced.");
    else
        length=snprintf(candidate,sizeof candidate,
            "Creation refused: %s. No new files or directories were reported.",status);
    if(length<0)return UMI_STATUS_IO_ERROR;
    if((size_t)length>=sizeof candidate||(size_t)length>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out,candidate,(size_t)length+1U);
    return UMI_STATUS_OK;
}
