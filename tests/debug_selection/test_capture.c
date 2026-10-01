/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_selection/test_capture.c
 * PURPOSE: Exercise captured debugger identities, stale records and repaired selection chains.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug/selection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];
    UmiProtocolTransport *transport=NULL;UmiProtocolClient *client=NULL;UmiDapClient dap;
    UmiDebugService *service=NULL;UmiDebugController *controller=NULL;UmiDebugWorkspace *workspace=NULL;
    OK(umi_protocol_transport_create_memory(16,&transport));OK(umi_protocol_client_create(transport,&client));
    OK(umi_protocol_client_start(client));OK(umi_dap_client_init(&dap,client));
    OK(umi_debug_service_create(&service));OK(umi_debug_controller_create(&dap,service,&controller));
    OK(umi_debug_workspace_create(service,controller,&workspace));
    UmiDebugThreadRegistry *threads=umi_debug_service_thread(service);
    UmiDebugStackFrameRegistry *frames=umi_debug_service_stack_frame(service);
    UmiDebugScopeRegistry *scopes=umi_debug_service_scope(service);
    UmiDebugThreadSnapshot thread={0};strcpy(thread.id,"thread");strcpy(thread.session_id,"session");thread.current=1;thread.stopped=1;
    UmiDebugStackFrameSnapshot frame={0};strcpy(frame.id,"frame");strcpy(frame.thread_id,thread.id);
    strcpy(frame.source_uri,"src/main.c");frame.line=7;frame.column=3;
    UmiDebugScopeSnapshot scope={0};strcpy(scope.id,"scope");strcpy(scope.frame_id,frame.id);
    UmiDebugSessionSnapshot session={0};strcpy(session.id,"session");strcpy(session.configuration_id,"launch");
    UmiDebugLaunchConfigurationSnapshot config={0};strcpy(config.id,"launch");strcpy(config.working_directory,"C:/reviewed/source");
    OK(umi_debug_thread_registry_upsert(threads,&thread));OK(umi_debug_stack_frame_registry_upsert(frames,&frame));
    OK(umi_debug_scope_registry_upsert(scopes,&scope));
    OK(umi_debug_session_registry_upsert(umi_debug_service_session(service),&session));
    OK(umi_debug_launch_configuration_registry_upsert(umi_debug_service_launch_configuration(service),&config));
    UmiDebugSelection *selection=NULL;UmiDebugSelectionSnapshot copied;
    OK(UmiDebugSelectionCapture(workspace,UMI_DEBUG_SELECT_FRAME,0,&selection));
    OK(UmiDebugSelectionValidate(workspace,selection,&copied));CHECK(copied.kind==UMI_DEBUG_SELECT_FRAME);
    CHECK(strcmp(copied.frame.id,"frame")==0&&strcmp(copied.sourceBase,config.working_directory)==0);
    UmiDebugViewStamp stamp,current;OK(UmiDebugWorkspaceViewStamp(workspace,&stamp));
    OK(UmiDebugWorkspaceViewStamp(workspace,&current));CHECK(UmiDebugViewStampEqual(&stamp,&current));
    int stale=0;
    if(strcmp(name,"ownership")==0){
        umi_debug_workspace_destroy(workspace);workspace=NULL;
        umi_debug_controller_destroy(controller);controller=NULL;
        umi_debug_service_destroy(service);service=NULL;
        OK(UmiDebugSelectionRead(selection,&copied));CHECK(copied.frame.line==7&&strcmp(copied.thread.id,"thread")==0);
        CHECK(strcmp(copied.sourceBase,"C:/reviewed/source")==0);
    }else if(strcmp(name,"owner-reuse")==0){
        umi_debug_workspace_destroy(workspace);workspace=NULL;
        OK(umi_debug_workspace_create(service,controller,&workspace));stale=1;
    }else if(strcmp(name,"replace")==0){frame.line=99;OK(umi_debug_stack_frame_registry_upsert(frames,&frame));stale=1;
    }else if(strcmp(name,"reuse")==0){OK(umi_debug_stack_frame_registry_remove(frames,frame.id));OK(umi_debug_stack_frame_registry_upsert(frames,&frame));stale=1;
    }else if(strcmp(name,"running")==0){
        thread.stopped=0;OK(umi_debug_thread_registry_upsert(threads,&thread));stale=1;
        UmiDebugSelection *rejected=selection;CHECK(UmiDebugSelectionCapture(workspace,UMI_DEBUG_SELECT_FRAME,0,&rejected)==UMI_STATUS_INVALID_STATE);CHECK(rejected==NULL);
    }else if(strcmp(name,"thread")==0){
        UmiDebugSelection *threadRow=NULL;OK(UmiDebugSelectionCapture(workspace,UMI_DEBUG_SELECT_THREAD,0,&threadRow));
        OK(UmiDebugSelectionRead(threadRow,&copied));CHECK(copied.kind==UMI_DEBUG_SELECT_THREAD&&copied.frame.id[0]=='\0');
        strcpy(thread.detail,"Changed stop reason");OK(umi_debug_thread_registry_upsert(threads,&thread));
        CHECK(UmiDebugSelectionValidate(workspace,threadRow,NULL)==UMI_STATUS_BUSY);UmiDebugSelectionDestroy(threadRow);stale=1;
    }else if(strcmp(name,"selection")==0){
        strcpy(frame.id,"frame2");OK(umi_debug_stack_frame_registry_upsert(frames,&frame));
        UmiDebugSelectionDestroy(selection);selection=NULL;OK(UmiDebugSelectionCapture(workspace,UMI_DEBUG_SELECT_FRAME,0,&selection));
        OK(umi_debug_workspace_select_frame(workspace,"frame2"));OK(umi_debug_workspace_select_frame(workspace,"frame"));stale=1;
    }else if(strcmp(name,"configuration")==0){
        strcpy(config.working_directory,"C:/different/source");OK(umi_debug_launch_configuration_registry_upsert(umi_debug_service_launch_configuration(service),&config));stale=1;
    }else if(strcmp(name,"session")==0){
        strcpy(session.state_text,"terminated");OK(umi_debug_session_registry_upsert(umi_debug_service_session(service),&session));stale=1;
    }else if(strcmp(name,"controller")==0){OK(umi_debug_controller_initialize(controller,"memory"));stale=1;
    }else if(strcmp(name,"unrelated")==0){
        UmiDebugVariableSnapshot variable={0};strcpy(variable.id,"variable");strcpy(variable.scope_id,"scope");strcpy(variable.value,"8");
        OK(umi_debug_variable_registry_upsert(umi_debug_service_variable(service),&variable));
        OK(UmiDebugWorkspaceViewStamp(workspace,&current));CHECK(!UmiDebugViewStampEqual(&stamp,&current));
        OK(UmiDebugSelectionValidate(workspace,selection,NULL));
        stamp=current;UmiDebugConsoleEntrySnapshot entry={0};strcpy(entry.id,"output");strcpy(entry.text,"progress");
        OK(umi_debug_console_entry_registry_upsert(umi_debug_service_console_entry(service),&entry));
        OK(UmiDebugWorkspaceViewStamp(workspace,&current));CHECK(UmiDebugViewStampEqual(&stamp,&current));
        OK(umi_debug_workspace_add_watch(workspace,"value",NULL,0));stale=1;
    }else if(strcmp(name,"invalid")==0){
        UmiDebugSelection *rejected=selection;
        CHECK(UmiDebugSelectionCapture(workspace,UMI_DEBUG_SELECT_FRAME,99,&rejected)==UMI_STATUS_NOT_FOUND);CHECK(rejected==NULL);
        rejected=selection;CHECK(UmiDebugSelectionCapture(workspace,(UmiDebugSelectionKind)99,0,&rejected)==UMI_STATUS_INVALID_ARGUMENT);CHECK(rejected==NULL);
        CHECK(UmiDebugSelectionValidate(NULL,selection,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!UmiDebugViewStampEqual(&stamp,NULL));
    }else if(strcmp(name,"repair-frame")==0){
        OK(umi_debug_stack_frame_registry_remove(frames,"frame"));strcpy(frame.id,"replacement");OK(umi_debug_stack_frame_registry_upsert(frames,&frame));
        UmiDebugWorkspaceSnapshot view;OK(umi_debug_workspace_snapshot(workspace,&view));
        CHECK(strcmp(view.selected_frame_id,"replacement")==0&&view.selected_scope_id[0]=='\0');stale=1;
    }else if(strcmp(name,"repair-thread")==0){
        OK(umi_debug_thread_registry_remove(threads,"thread"));strcpy(thread.id,"new-thread");OK(umi_debug_thread_registry_upsert(threads,&thread));
        strcpy(frame.id,"new-frame");strcpy(frame.thread_id,thread.id);OK(umi_debug_stack_frame_registry_upsert(frames,&frame));
        UmiDebugWorkspaceSnapshot view;OK(umi_debug_workspace_snapshot(workspace,&view));
        CHECK(strcmp(view.selected_thread_id,thread.id)==0&&strcmp(view.selected_frame_id,frame.id)==0&&view.selected_scope_id[0]=='\0');stale=1;
    }else if(strcmp(name,"repair-scope")==0){
        OK(umi_debug_scope_registry_remove(scopes,"scope"));strcpy(scope.id,"new-scope");OK(umi_debug_scope_registry_upsert(scopes,&scope));
        UmiDebugWorkspaceSnapshot view;OK(umi_debug_workspace_snapshot(workspace,&view));CHECK(strcmp(view.selected_scope_id,scope.id)==0);stale=1;
    }else if(strcmp(name,"orphan")==0){
        umi_debug_thread_registry_clear(threads);UmiDebugWorkspaceSnapshot view;OK(umi_debug_workspace_snapshot(workspace,&view));
        CHECK(view.selected_thread_id[0]=='\0'&&view.selected_frame_id[0]=='\0'&&view.selected_scope_id[0]=='\0');
        UmiDebugSelection *rejected=NULL;CHECK(UmiDebugSelectionCapture(workspace,UMI_DEBUG_SELECT_FRAME,0,&rejected)!=UMI_STATUS_OK);CHECK(rejected==NULL);stale=1;
    }else return 2;
    if(stale){
        memset(&copied,0x5a,sizeof(copied));UmiDebugSelectionSnapshot before=copied;
        CHECK(UmiDebugSelectionValidate(workspace,selection,&copied)==UMI_STATUS_BUSY);CHECK(memcmp(&before,&copied,sizeof(copied))==0);
        OK(UmiDebugSelectionRead(selection,&copied));CHECK(copied.frame.line==7);
    }
    UmiDebugSelectionDestroy(selection);umi_debug_workspace_destroy(workspace);umi_debug_controller_destroy(controller);
    umi_debug_service_destroy(service);umi_protocol_client_destroy(client);umi_protocol_transport_destroy(transport);return 0;
}
