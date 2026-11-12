/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_creation_message.c
 *
 * PURPOSE:
 *   Qualify bounded checked-creation descriptions, report immutability and honest partial-work wording without I/O.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/developer_project/generator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *mode=argv[1],*required="Project source created:",*forbidden=NULL;
    UmiStatus result=UMI_STATUS_OK,expected=UMI_STATUS_OK;
    UmiDeveloperProjectGeneratorReport report={2U,3U,0U,9U,0},saved;
    const UmiDeveloperProjectGeneratorReport *selected=&report;
    char out[1024],previous[1024];memset(out,'x',sizeof out);out[sizeof out-1U]='\0';memcpy(previous,out,sizeof out);
    size_t capacity=sizeof out;char *destination=out;
    if(!strcmp(mode,"success")||!strcmp(mode,"report-preserved")){}
    else if(!strcmp(mode,"preview")||!strcmp(mode,"preview-refusal")){
        report.dry_run=1;report.directories_created=0U;
        required=!strcmp(mode,"preview")?"Preview prepared:":"Preview refused:";
        if(!strcmp(mode,"preview-refusal"))result=UMI_STATUS_INVALID_ARGUMENT;
        forbidden="Project source created:";
    }else if(!strcmp(mode,"existing")){
        memset(&report,0,sizeof report);result=UMI_STATUS_ALREADY_EXISTS;
        required="destination name already exists";forbidden="Partial work";
    }else if(!strcmp(mode,"partial-directory")||!strcmp(mode,"partial-file")||!strcmp(mode,"partial-collision")){
        if(!strcmp(mode,"partial-directory")){report.files_created=0U;report.bytes_written=0U;}
        result=!strcmp(mode,"partial-collision")?UMI_STATUS_ALREADY_EXISTS:UMI_STATUS_IO_ERROR;
        required="Partial work was retained";forbidden="That destination name already exists";
    }else if(!strcmp(mode,"refused")){
        memset(&report,0,sizeof report);result=UMI_STATUS_PERMISSION_DENIED;
        required="No new files or directories were reported";
    }else if(!strcmp(mode,"unknown-progress")){
        selected=NULL;result=UMI_STATUS_IO_ERROR;required="Progress is unavailable";
        forbidden="No new files";
    }else if(!strcmp(mode,"capacity")){capacity=8U;expected=UMI_STATUS_CAPACITY_EXCEEDED;}
    else if(!strcmp(mode,"exact-capacity")){
        CHECK(UmiDeveloperProjectCreationDescribe(result,&report,out,sizeof out)==UMI_STATUS_OK);
        capacity=strlen(out)+1U;memset(out,'x',sizeof out);out[sizeof out-1U]='\0';
        memcpy(previous,out,sizeof out);
    }else if(!strcmp(mode,"null-output")){destination=NULL;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"zero-capacity")){capacity=0U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"large-counts")){report.directories_created=SIZE_MAX;report.files_created=SIZE_MAX;report.bytes_written=SIZE_MAX;}
    else if(!strcmp(mode,"skipped-report")){report.files_skipped=1U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"invalid-preview")){report.dry_run=1;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"invalid-flag")){report.dry_run=2;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else if(!strcmp(mode,"bytes-without-file")){report.files_created=0U;expected=UMI_STATUS_INVALID_ARGUMENT;}
    else return 2;
    memcpy(&saved,&report,sizeof report);
    CHECK(UmiDeveloperProjectCreationDescribe(result,selected,destination,capacity)==expected);
    CHECK(memcmp(&saved,&report,sizeof report)==0);
    if(expected==UMI_STATUS_OK){
        CHECK(strstr(out,required)!=NULL);
        if(forbidden!=NULL)CHECK(strstr(out,forbidden)==NULL);
        CHECK(strlen(out)<capacity);
    }else CHECK(memcmp(out,previous,sizeof out)==0);
    return 0;
}
