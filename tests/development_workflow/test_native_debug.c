/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/development_workflow/test_native_debug.c
 * PURPOSE: Exercise an installed LLDB DAP process against a real Notes executable.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/platform.h"
#include "umicom/platform/clock.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define REQUIRE(call) do { UmiStatus s_ = (call); fprintf(stderr, "%s: %s\n", #call, umi_status_text(s_)); if(s_!=UMI_STATUS_OK) {result=1;goto cleanup;} } while(0)
int main(int argc, char **argv) {
    if(argc!=6) return 2;
    if(strcmp(argv[1],"unavailable-runtime-dependency")==0) { fprintf(stderr,"Native debugger is unavailable or its runtime probe failed; no real-debugger result.\n"); return 77; }
    int result=0;
    UmiDebugRuntimePlatform *platform=NULL;
    REQUIRE(umi_debug_runtime_platform_create(&platform));
    UmiDebugService *service=umi_debug_runtime_platform_service(platform);
    UmiDebugAdapterProfile profile=*umi_debug_runtime_builtin_profile_find(strcmp(argv[5],"gdb")==0 ? "debug.adapter.gdb-dap" : "debug.adapter.lldb-dap");
    if(strlen(argv[1])>=sizeof(profile.executable)) {result=2;goto cleanup;}
    (void)snprintf(profile.executable,sizeof(profile.executable),"%s",argv[1]);
    REQUIRE(umi_debug_adapter_profile_registry_upsert(umi_debug_service_adapter_profiles(service), &profile));
    UmiDebugBreakpointSnapshot bp={0};
    bp.struct_size=sizeof(bp); bp.api_version=UMI_DEBUG_BREAKPOINT_API_VERSION;
    (void)snprintf(bp.id,sizeof(bp.id),"notes.total");
    (void)snprintf(bp.uri,sizeof(bp.uri),"%s",argv[3]);
    bp.line=(uint32_t)strtoul(argv[4],NULL,10);bp.enabled=1;
    REQUIRE(umi_debug_breakpoint_registry_upsert(umi_debug_service_breakpoint(service),&bp));
    char arguments[8192];
    /* Paths in this portable fixture arrive from CMake, escaped by the real JSON writer. */
    UmiLanguageRuntimeJsonWriter w;
    umi_language_runtime_json_writer_init(&w,arguments,sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&w,"{\"program\":");
    (void)umi_language_runtime_json_writer_string(&w,argv[2]);
    (void)umi_language_runtime_json_writer_raw(&w,strcmp(argv[5],"gdb")==0 ? "}" : ",\"disableASLR\":false,\"stopOnEntry\":false}");
    REQUIRE(w.status);
    REQUIRE(umi_debug_runtime_platform_start(platform,profile.id,"notes.debug.session","notes.debug.launch",arguments,0,NULL,5000U));
    UmiDebugRuntimePlatformSnapshot snapshot;
    UmiClock clock=umi_clock_system();
    uint64_t begin=clock.monotonic_nanoseconds(&clock);
    do {
        int handled=0;
        UmiStatus poll=umi_debug_runtime_platform_pump_event(platform,50U,&handled);
        if(poll!=UMI_STATUS_OK && poll!=UMI_STATUS_NOT_FOUND){fprintf(stderr,"poll: %s\n",umi_status_text(poll));result=1;goto cleanup;}
        REQUIRE(umi_debug_runtime_platform_snapshot(platform,&snapshot));
    }while(!snapshot.paused && clock.monotonic_nanoseconds(&clock)-begin<5000000000ULL);
    if(!snapshot.paused){fprintf(stderr,"No breakpoint stop\n");result=1;goto cleanup;}
    REQUIRE(umi_debug_runtime_platform_refresh_threads(platform,3000U));
    REQUIRE(umi_debug_runtime_platform_refresh_stack(platform,snapshot.active_thread_id,3000U));
    REQUIRE(umi_debug_runtime_platform_snapshot(platform,&snapshot));
    fprintf(stderr,"thread=%llu frame=%llu\n",(unsigned long long)snapshot.active_thread_id,(unsigned long long)snapshot.active_frame_id);
    REQUIRE(umi_debug_runtime_platform_refresh_scopes(platform,snapshot.active_frame_id,3000U));
    for(size_t i=0;i<umi_debug_scope_registry_count(umi_debug_service_scope(service));++i) {
        UmiDebugScopeSnapshot scope;
        REQUIRE(umi_debug_scope_registry_at(umi_debug_service_scope(service),i,&scope));
        fprintf(stderr,"scope %s %s ref %llu\n",scope.id,scope.name,(unsigned long long)scope.variables_reference);
        if(!scope.expensive && scope.variables_reference)
            REQUIRE(umi_debug_runtime_platform_refresh_variables(platform,scope.id,scope.variables_reference,3000U));
    }
    int found=0;
    for(size_t i=0;i<umi_debug_variable_registry_count(umi_debug_service_variable(service));++i){
        UmiDebugVariableSnapshot v;
        REQUIRE(umi_debug_variable_registry_at(umi_debug_service_variable(service),i,&v));
        fprintf(stderr,"%s=%s\n",v.name,v.value);
        if(strcmp(v.name,"savedNotes")==0 && strcmp(v.value,"2")==0) found=1;
    }
    if(!found){fprintf(stderr,"Expected savedNotes=2\n");result=1;}
cleanup:
    umi_debug_runtime_platform_destroy(platform);
    return result;
}
