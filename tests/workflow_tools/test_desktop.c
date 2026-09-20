/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workflow_tools/test_desktop.c
 * PURPOSE:
 *   Verify non-mutating application categories, including preserved hidden selections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/application_filter.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)

static int Run(const char *name)
{
    UmiApplicationLaunchChoice choice={0}, before;
    choice.state=UMI_APPLICATION_RUNTIME_STOPPED;
    choice.selected=true;choice.eligible=true;
    strcpy(choice.application_id,"org.umicom.studio");
    before=choice;
    bool match=false;
    if(strcmp(name,"all")==0) {
        CHECK(UmiDeskApplicationFilterMatches(&choice,UMI_DESK_APPLICATION_FILTER_ALL,&match)==UMI_STATUS_OK && match);
    } else if(strcmp(name,"available")==0) {
        CHECK(UmiDeskApplicationFilterMatches(&choice,UMI_DESK_APPLICATION_FILTER_AVAILABLE,&match)==UMI_STATUS_OK && match);
        choice.eligible=false;
        CHECK(UmiDeskApplicationFilterMatches(&choice,UMI_DESK_APPLICATION_FILTER_AVAILABLE,&match)==UMI_STATUS_OK && !match);
        choice.eligible=true;
    } else if(strcmp(name,"running")==0) {
        CHECK(UmiDeskApplicationFilterMatches(&choice,UMI_DESK_APPLICATION_FILTER_RUNNING,&match)==UMI_STATUS_OK && !match);
        CHECK(choice.selected);
        choice.running=true;choice.state=UMI_APPLICATION_RUNTIME_ATTENTION;
        CHECK(UmiDeskApplicationFilterMatches(&choice,UMI_DESK_APPLICATION_FILTER_RUNNING,&match)==UMI_STATUS_OK && match);
        choice=before;
    } else if(strcmp(name,"selected")==0) {
        choice.eligible=false;
        CHECK(UmiDeskApplicationFilterMatches(&choice,UMI_DESK_APPLICATION_FILTER_SELECTED,&match)==UMI_STATUS_OK && match);
        CHECK(choice.selected);choice=before;
    } else if(strcmp(name,"attention")==0) {
        for(int state=UMI_APPLICATION_RUNTIME_UNKNOWN;state<=UMI_APPLICATION_RUNTIME_STOPPING;++state) {
            choice.state=(UmiApplicationRuntimeState)state;
            CHECK(UmiDeskApplicationFilterMatches(&choice,UMI_DESK_APPLICATION_FILTER_ATTENTION,&match)==UMI_STATUS_OK);
            CHECK(match==(state==UMI_APPLICATION_RUNTIME_FAILED || state==UMI_APPLICATION_RUNTIME_ATTENTION));
        }
        choice=before;
    } else if(strcmp(name,"invalid")==0) {
        match=true;
        CHECK(UmiDeskApplicationFilterMatches(&choice,(UmiDeskApplicationFilter)99,&match)==UMI_STATUS_INVALID_ARGUMENT && match);
        CHECK(UmiDeskApplicationFilterMatches(NULL,UMI_DESK_APPLICATION_FILTER_ALL,&match)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDeskApplicationFilterMatches(&choice,UMI_DESK_APPLICATION_FILTER_ALL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        choice.state=(UmiApplicationRuntimeState)99;
        CHECK(UmiDeskApplicationFilterMatches(&choice,UMI_DESK_APPLICATION_FILTER_ALL,&match)==UMI_STATUS_INVALID_ARGUMENT);
        choice=before;
    } else return 2;
    CHECK(memcmp(&choice,&before,sizeof choice)==0);
    return 0;
}
int main(int argc,char **argv){return argc==2?Run(argv[1]):2;}
