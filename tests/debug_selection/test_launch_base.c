/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_selection/test_launch_base.c
 * PURPOSE: Check the native launch entry point publishes the reviewed source directory using a controlled DAP peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_runtime/platform.h"
#include "umicom/debug/selection.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);result=1;goto cleanup;}}while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
int main(int argc,char **argv)
{
    if(argc!=2)return 2;int result=0;
    UmiDebugRuntimePlatform *platform=NULL;UmiProtocolTransport *transport=NULL;UmiProtocolClient *client=NULL;
    UmiDebugController *controller=NULL;UmiDebugWorkspace *workspace=NULL;UmiDebugSelection *selection=NULL;UmiDapClient dap;
    char directory[UMI_PATH_CAPACITY];OK(umi_fs_temp_directory(directory,sizeof(directory)));
    OK(umi_debug_runtime_platform_create(&platform));
    /* This executable speaks deterministic DAP and does not run a debuggee.
     * LaunchNative still exercises the real profile/process/request composition. */
    char excessive[1025];memset(excessive,' ',sizeof(excessive)-1);excessive[sizeof(excessive)-1]='\0';
    CHECK(UmiDebugRuntimePlatformLaunchNative(platform,"lldb",argv[1],argv[1],directory,excessive,2000)==UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_debug_launch_configuration_registry_count(umi_debug_service_launch_configuration(
        umi_debug_runtime_platform_service(platform)))==0);
    const char *arguments="one \"two words\"";
    OK(UmiDebugRuntimePlatformLaunchNative(platform,"lldb",argv[1],argv[1],directory,arguments,2000));
    UmiDebugService *model=umi_debug_runtime_platform_service(platform);UmiDebugLaunchConfigurationSnapshot configuration;
    OK(umi_debug_launch_configuration_registry_find(umi_debug_service_launch_configuration(model),"native.launch",&configuration));
    CHECK(strcmp(configuration.working_directory,directory)==0&&strcmp(configuration.program,argv[1])==0);
    CHECK(strcmp(configuration.arguments,arguments)==0&&configuration.stop_on_entry);
    UmiDebugRuntimePlatformSnapshot state={0};
    for(size_t i=0;i<40;++i){
        OK(umi_debug_runtime_platform_snapshot(platform,&state));if(state.paused)break;
        int handled=0;UmiStatus status=umi_debug_runtime_platform_pump_event(platform,25,&handled);
        CHECK(status==UMI_STATUS_OK||status==UMI_STATUS_NOT_FOUND);
    }
    CHECK(state.paused);OK(UmiDebugRuntimePlatformInspectStopped(platform,2000));
    OK(umi_protocol_transport_create_memory(16,&transport));OK(umi_protocol_client_create(transport,&client));
    OK(umi_protocol_client_start(client));OK(umi_dap_client_init(&dap,client));OK(umi_debug_controller_create(&dap,model,&controller));
    OK(umi_debug_workspace_create(model,controller,&workspace));
    OK(UmiDebugSelectionCapture(workspace,UMI_DEBUG_SELECT_FRAME,0,&selection));UmiDebugSelectionSnapshot copied;
    OK(UmiDebugSelectionRead(selection,&copied));CHECK(strcmp(copied.sourceBase,directory)==0&&strcmp(copied.frame.source_uri,"notes.c")==0);
    CHECK(strcmp(copied.thread.session_id,state.active_session_id)==0);
cleanup:
    UmiDebugSelectionDestroy(selection);umi_debug_workspace_destroy(workspace);umi_debug_controller_destroy(controller);
    umi_protocol_client_destroy(client);umi_protocol_transport_destroy(transport);umi_debug_runtime_platform_destroy(platform);return result;
}
