/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_project_cli.c
 *
 * PURPOSE:
 *   Qualify native project parsing, write-free previews and explicit fresh-destination creation through real Framework services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../../tools/umicom/src/cli.h"
#include "umicom/developer_project/new_project.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"%d: %s\n",__LINE__,#c);return 1;}}while(0)
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    const char *name=argv[1];
    CHECK(umi_fs_is_absolute(argv[2]));
    CHECK(umi_fs_make_directories(argv[2])==UMI_STATUS_OK);
    char folder[32],destination[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
    int chosen=0;
    /* Preserve previous runs as owner evidence. Select a new bounded fixture
     * name instead of recursively deleting an existing test directory. */
    for(unsigned i=1U;i<10000U;++i){
        snprintf(folder,sizeof folder,"fixture%04u",i);
        CHECK(UmiDeveloperProjectChooseDestination(argv[2],folder,destination,sizeof destination)==UMI_STATUS_OK);
        if(!umi_fs_exists(destination)){chosen=1;break;}
    }
    CHECK(chosen);
    char *args[18]={"plan","--template","developer.template.c23-console","--name","Umicom Example",
        "--id","org.umicom.example","--target","umicom_example","--parent",argv[2],"--folder",folder};
    int count=13,expected=2,written=0,existing=0;
    char long_name[UMI_DEVELOPER_PROJECT_NAME_CAPACITY+1U];
    memset(long_name,'a',sizeof long_name-1U);long_name[sizeof long_name-1U]='\0';
    if(!strcmp(name,"help")){args[0]="--help";count=1;expected=0;}
    else if(!strcmp(name,"templates")){args[0]="templates";count=1;expected=0;}
    else if(!strcmp(name,"templates-extra")){args[0]="templates";count=2;}
    else if(!strcmp(name,"plan"))expected=0;
    else if(!strcmp(name,"plan-files")){args[count++]="--show-files";expected=0;}
    else if(!strcmp(name,"plan-c23")){args[2]="developer.template.c23-gtk4";expected=0;}
    else if(!strcmp(name,"plan-cpp")){args[2]="developer.template.cpp-gtk4";expected=0;}
    else if(!strcmp(name,"plan-thin")){args[2]="developer.template.thin-desktop-application";expected=0;}
    else if(!strcmp(name,"unknown-option")){args[count++]="--overwrite";args[count++]="yes";}
    else if(!strcmp(name,"duplicate-option")){args[count++]="--name";args[count++]="Again";}
    else if(!strcmp(name,"missing-value"))count=12;
    else if(!strcmp(name,"missing-option"))count=11;
    else if(!strcmp(name,"missing-apply"))args[0]="create";
    else if(!strcmp(name,"plan-apply"))args[count++]="--apply";
    else if(!strcmp(name,"relative-parent"))args[10]="relative";
    else if(!strcmp(name,"escape-folder"))args[12]="../outside";
    else if(!strcmp(name,"reserved-folder"))args[12]="CON";
    else if(!strcmp(name,"unknown-template")){args[2]="developer.template.missing";expected=1;}
    else if(!strcmp(name,"oversized-name"))args[4]=long_name;
    else if(!strcmp(name,"create")){args[0]="create";args[count++]="--apply";expected=0;written=1;}
    else if(!strcmp(name,"existing-directory")||!strcmp(name,"existing-file")){
        args[0]="create";args[count++]="--apply";expected=1;existing=1;
        if(!strcmp(name,"existing-directory"))CHECK(umi_fs_make_directories(destination)==UMI_STATUS_OK);
        else CHECK(umi_fs_write_text(destination,"existing owner content")==UMI_STATUS_OK);
    } else if(!strcmp(name,"null-vector")){
        CHECK(umi_cli_command_project(1,NULL)==2);
        CHECK(!umi_fs_exists(destination));return 0;
    } else if(!strcmp(name,"empty-argument"))args[4]="";
    else if(!strcmp(name,"unknown-command"))args[0]="unknown";
    else return 2;
    CHECK(umi_cli_command_project(count,args)==expected);
    if(written){
        char source[UMI_DEVELOPER_PROJECT_PATH_CAPACITY],git[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
        CHECK(umi_fs_join(source,sizeof source,destination,"src/main.c")==UMI_STATUS_OK);
        CHECK(umi_fs_join(git,sizeof git,destination,".git")==UMI_STATUS_OK);
        char *text=NULL;size_t bytes=0U;
        CHECK(umi_fs_read_text(source,&text,&bytes)==UMI_STATUS_OK);
        int valid=text!=NULL&&strstr(text,"Umicom Example")!=NULL;
        free(text);CHECK(valid&&!umi_fs_exists(git));
    } else if(existing){
        CHECK(umi_fs_exists(destination));
        if(!strcmp(name,"existing-file")){
            char *text=NULL;size_t bytes=0U;
            CHECK(umi_fs_read_text(destination,&text,&bytes)==UMI_STATUS_OK);
            int same=text!=NULL&&!strcmp(text,"existing owner content");free(text);CHECK(same);
        } else {
            char file[UMI_DEVELOPER_PROJECT_PATH_CAPACITY];
            CHECK(umi_fs_join(file,sizeof file,destination,"CMakeLists.txt")==UMI_STATUS_OK);
            CHECK(!umi_fs_exists(file));
        }
    } else CHECK(!umi_fs_exists(destination));
    return 0;
}
