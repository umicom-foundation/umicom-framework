/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/development_workflow/test_debug_session.c
 * PURPOSE: Verify production process/DAP integration against the controlled peer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/platform.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c);result=1;goto done;} }while(0)
#define OK(c) do{UmiStatus s_=(c);if(s_!=UMI_STATUS_OK){fprintf(stderr,"%s: %s\n",#c,umi_status_text(s_));result=1;goto done;}}while(0)
static UmiStatus WaitStopped(UmiDebugRuntimePlatform *p) {
    for(unsigned i=0;i<40U;++i) {
        UmiDebugRuntimePlatformSnapshot s;
        UmiStatus status=umi_debug_runtime_platform_snapshot(p,&s);
        if(status!=UMI_STATUS_OK || s.paused) return status;
        int handled=0; status=umi_debug_runtime_platform_pump_event(p,25U,&handled);
        if(status!=UMI_STATUS_OK && status!=UMI_STATUS_NOT_FOUND) return status;
    }
    return UMI_STATUS_TIMEOUT;
}
int main(int argc,char **argv) {
    if(argc!=3) return 2;
    const char *mode=argv[2];int result=0;
    UmiDebugRuntimePlatform *platform=NULL;
    OK(umi_debug_runtime_platform_create(&platform));
    UmiDebugService *service=umi_debug_runtime_platform_service(platform);
    if(strcmp(mode,"invalid-launch")==0) {
        CHECK(UmiDebugRuntimePlatformLaunchNative(NULL,"lldb","","/missing","/","",100U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDebugRuntimePlatformLaunchNative(platform,"unknown","","/missing","/","",100U)==UMI_STATUS_NOT_IMPLEMENTED);
        CHECK(UmiDebugRuntimePlatformInspectStopped(platform,50U)==UMI_STATUS_INVALID_STATE);
        goto done;
    }
    UmiDebugAdapterProfile profile=*umi_debug_runtime_builtin_profile_find("debug.adapter.lldb-dap");
    (void)snprintf(profile.executable,sizeof(profile.executable),"%s",argv[1]);
    (void)snprintf(profile.arguments,sizeof(profile.arguments),"%s",mode);
    OK(umi_debug_adapter_profile_registry_upsert(umi_debug_service_adapter_profiles(service),&profile));
    if(strcmp(mode,"initial-breakpoints")==0 || strcmp(mode,"disabled-breakpoints")==0){
        UmiDebugBreakpointSnapshot bp={0};bp.struct_size=sizeof(bp);bp.api_version=UMI_DEBUG_BREAKPOINT_API_VERSION;
        (void)snprintf(bp.uri,sizeof(bp.uri),"notes.c");bp.line=13;bp.enabled=0;
        if(strcmp(mode,"disabled-breakpoints")==0){(void)snprintf(bp.id,sizeof(bp.id),"disabled");OK(umi_debug_breakpoint_registry_upsert(umi_debug_service_breakpoint(service),&bp));}
        bp.enabled=1;(void)snprintf(bp.id,sizeof(bp.id),"active");
        OK(umi_debug_breakpoint_registry_upsert(umi_debug_service_breakpoint(service),&bp));
    }
    UmiDebugRuntimeProfileHealth health; OK(umi_debug_runtime_profile_health_probe(&profile,&health));
    fprintf(stderr,"health %d %s resolved %s\n",health.available,umi_status_text(health.status),health.resolved_path);
    UmiClock clock=umi_clock_system(); uint64_t start=clock.monotonic_nanoseconds(&clock);
    UmiStatus launched=umi_debug_runtime_platform_start(platform,profile.id,"test.session","test.launch","{\"program\":\"fixture-only\"}",0,NULL,350U);
    fprintf(stderr,"launch result: %s\n",umi_status_text(launched));
    if(strcmp(mode,"no-initialized")==0){CHECK(launched==UMI_STATUS_TIMEOUT);CHECK(clock.monotonic_nanoseconds(&clock)-start<1500000000ULL);goto done;}
    if(strcmp(mode,"reject-launch")==0){CHECK(launched==UMI_STATUS_UNAVAILABLE);goto done;}
    CHECK(launched==UMI_STATUS_OK); OK(WaitStopped(platform));
    OK(UmiDebugRuntimePlatformInspectStopped(platform,1000U));
    UmiDebugRuntimePlatformSnapshot snapshot;OK(umi_debug_runtime_platform_snapshot(platform,&snapshot));
    CHECK(snapshot.paused && snapshot.active_frame_id==0U);
    CHECK(umi_debug_thread_registry_count(umi_debug_service_thread(service))==1U);
    CHECK(umi_debug_stack_frame_registry_count(umi_debug_service_stack_frame(service))==1U);
    UmiDebugVariableSnapshot variable;OK(umi_debug_variable_registry_at(umi_debug_service_variable(service),0,&variable));
    CHECK(strcmp(variable.name,"savedNotes")==0 && strcmp(variable.value,"2")==0);
    if(strcmp(mode,"disabled-breakpoints")==0){
        UmiDebugBreakpointSnapshot bp;OK(umi_debug_breakpoint_registry_find(umi_debug_service_breakpoint(service),"disabled",&bp));CHECK(!bp.verified);
        OK(umi_debug_breakpoint_registry_find(umi_debug_service_breakpoint(service),"active",&bp));CHECK(bp.verified);
    }
    if(strcmp(mode,"watch-frame-zero")==0){
        OK(umi_debug_runtime_platform_evaluate_watch(platform,"notes.watch","savedNotes",0U,500U));
        UmiDebugWatchSnapshot watch;OK(umi_debug_watch_registry_find(umi_debug_service_watch(service),"notes.watch",&watch));CHECK(strcmp(watch.value,"2")==0);
    }
    if(strcmp(mode,"step-over")==0 || strcmp(mode,"step-into")==0 || strcmp(mode,"step-out")==0){
        if(strcmp(mode,"step-over")==0)OK(umi_debug_runtime_platform_step_over(platform,1U,500U));
        if(strcmp(mode,"step-into")==0)OK(umi_debug_runtime_platform_step_into(platform,1U,500U));
        if(strcmp(mode,"step-out")==0)OK(umi_debug_runtime_platform_step_out(platform,1U,500U));
        OK(WaitStopped(platform));OK(UmiDebugRuntimePlatformInspectStopped(platform,1000U));
        OK(umi_debug_variable_registry_at(umi_debug_service_variable(service),0U,&variable));CHECK(strcmp(variable.value,"5")==0);
    }
    if(strcmp(mode,"continue-exit")==0){
        OK(umi_debug_runtime_platform_continue(platform,1U,500U));
        for(unsigned i=0;i<16U;++i){int handled=0;UmiStatus poll=umi_debug_runtime_platform_pump_event(platform,20U,&handled);CHECK(poll==UMI_STATUS_OK || poll==UMI_STATUS_NOT_FOUND);}
        OK(umi_debug_runtime_platform_snapshot(platform,&snapshot));CHECK(snapshot.adapter.state==UMI_DEBUG_RUNTIME_ADAPTER_STOPPED);
    }
    OK(umi_debug_runtime_platform_stop(platform,1,500U));
    OK(umi_debug_runtime_platform_snapshot(platform,&snapshot));CHECK(!snapshot.active);
    if(strcmp(mode,"restart")==0){
        OK(umi_debug_runtime_platform_start(platform,profile.id,"test.session2","test.launch","{}",0,NULL,500U));OK(WaitStopped(platform));
        OK(umi_debug_runtime_platform_stop(platform,1,500U));
    }
done:
    umi_debug_runtime_platform_destroy(platform);return result;
}
