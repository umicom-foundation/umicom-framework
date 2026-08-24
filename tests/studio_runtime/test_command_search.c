/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_runtime/test_command_search.c
 *
 * PURPOSE:
 *   Behavioral regression coverage for Studio runtime command search.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/studio_runtime/command_search.h"
static UmiStatus handler(void*u,const char*a,char*m,size_t c)
{(void)u;(void)a;if(m&&c)m[0]='\0';return UMI_STATUS_OK;}
int main(void)
{
    UmiCommandRegistry *commands=NULL;
    UmiApplicationShellRegistry *shell=NULL;
    UmiCommandDescriptor command={0};
    UmiStudioRuntimeBindings bindings={0};
    UmiStudioRuntimeCommandSearchResults results;
    assert(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    assert(umi_application_shell_registry_create(&shell)==UMI_STATUS_OK);
    command.structure_size=(uint32_t)sizeof(command);
    command.command_id="test.build";command.title="Build Project";
    command.category="Build";command.description="Build";command.handler=handler;
    assert(umi_command_registry_register(commands,&command)==UMI_STATUS_OK);
    bindings.commands=commands;bindings.shell_registry=shell;
    assert(umi_studio_command_search(&bindings,"build",&results)==UMI_STATUS_OK);
    assert(results.count==1U);
    assert(strcmp(results.items[0].command_id,"test.build")==0);
    umi_application_shell_registry_destroy(shell);
    umi_command_registry_destroy(commands);
    return 0;
}

