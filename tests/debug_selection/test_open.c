/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_selection/test_open.c
 * PURPOSE: Check stack-frame source coordinates and failed-open draft preservation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/debug_ui/navigation.h"
#include "umicom/document/local_provider.h"
#include "umicom/document/uri.h"
#include "umicom/platform/filesystem.h"
#include "umicom/ui/workbench.h"
#ifdef _WIN32
#include <process.h>
#define GETPID _getpid
#else
#include <unistd.h>
#define GETPID getpid
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];
    char temp[UMI_PATH_CAPACITY],root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY],leaf[100];
    OK(umi_fs_temp_directory(temp,sizeof(temp)));(void)snprintf(leaf,sizeof(leaf),"umicom-debug-source-%ld-%s",(long)GETPID(),name);
    OK(umi_fs_join(root,sizeof(root),temp,leaf));CHECK(!umi_fs_exists(root));OK(umi_fs_make_directories(root));
    OK(umi_fs_join(path,sizeof(path),root,"source notes.c"));const char *saved="first\nA\xc2\xa3" "B\nlast\n";OK(umi_fs_write_text(path,saved));
    UmiCommandRegistry *commands=NULL;UmiUiWorkbench *workbench=NULL;UmiDocumentStore *store=NULL;UmiDocumentCoordinator *documents=NULL;
    OK(umi_command_registry_create(&commands));OK(umi_ui_workbench_create("debug.source",commands,&workbench));OK(umi_document_store_create(&store));
    UmiDocumentProvider provider=umi_document_local_provider();OK(umi_document_coordinator_create(store,workbench,&provider,&documents));
    char view[UMI_UI_ID_CAPACITY];OK(umi_document_coordinator_open(documents,path,view,sizeof(view)));
    UmiUiDocumentViewModel *views=umi_ui_workbench_documents(workbench);UmiUiDocumentViewSnapshot before,after;
    OK(umi_ui_document_view_model_find(views,view,&before));before.cursor_offset=1;before.selection_length=2;
    OK(umi_ui_document_view_model_upsert(views,&before));
    UmiDebugStackFrameSnapshot frame={0};strcpy(frame.id,"frame");strcpy(frame.source_uri,path);frame.line=2;frame.column=4;
    size_t offset=9876;int moved=0;const char *draft="first\nunsaved draft\nlast\n";
    if(strcmp(name,"absolute")==0||strcmp(name,"relative")==0||strcmp(name,"draft")==0||strcmp(name,"file-uri")==0){
        if(strcmp(name,"relative")==0)strcpy(frame.source_uri,"source notes.c");
        if(strcmp(name,"draft")==0){before.dirty=1;OK(UmiUiDocumentViewModelUpsertText(views,&before,draft,strlen(draft)));}
        if(strcmp(name,"file-uri")==0)OK(umi_document_uri_from_path(path,frame.source_uri,sizeof(frame.source_uri)));
        OK(UmiDebugFrameOpenSource(documents,&frame,root,&offset));CHECK(offset==9);moved=1;
    }else if(strcmp(name,"missing-file")==0){
        OK(umi_fs_join(frame.source_uri,sizeof(frame.source_uri),root,"missing.c"));CHECK(UmiDebugFrameOpenSource(documents,&frame,root,&offset)!=UMI_STATUS_OK);
    }else if(strcmp(name,"missing-line")==0){frame.line=999;CHECK(UmiDebugFrameOpenSource(documents,&frame,root,&offset)==UMI_STATUS_NOT_FOUND);
    }else if(strcmp(name,"explicit-base")==0){
        strcpy(frame.source_uri,"source notes.c");CHECK(UmiDebugFrameOpenSource(documents,&frame,NULL,&offset)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDebugFrameOpenSource(documents,&frame,"relative",&offset)==UMI_STATUS_INVALID_ARGUMENT);
    }else if(strcmp(name,"invalid")==0){
        strcpy(frame.source_uri,"https://example.test/main.c");CHECK(UmiDebugFrameOpenSource(documents,&frame,root,&offset)==UMI_STATUS_NOT_FOUND);
        memset(frame.source_uri,'x',sizeof(frame.source_uri));CHECK(UmiDebugFrameOpenSource(documents,&frame,root,&offset)==UMI_STATUS_INVALID_ARGUMENT);
        strcpy(frame.source_uri,path);frame.line=0;CHECK(UmiDebugFrameOpenSource(documents,&frame,root,&offset)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiDebugFrameOpenSource(documents,NULL,root,&offset)==UMI_STATUS_INVALID_ARGUMENT);
    }else return 2;
    OK(umi_ui_document_view_model_find(views,view,&after));CHECK(after.cursor_offset==(moved?9U:1U)&&after.selection_length==(moved?0U:2U));
    CHECK(offset==(moved?9U:9876U));CHECK(umi_document_coordinator_count(documents)==1);
    UmiDocumentWorkingCopySnapshot active;OK(umi_document_coordinator_active_snapshot(documents,&active));CHECK(strcmp(active.view_id,view)==0);
    char *text=NULL;size_t bytes=0;OK(UmiUiDocumentViewModelCopyText(views,view,&text,&bytes));CHECK(strcmp(text,strcmp(name,"draft")==0?draft:saved)==0);UmiUiDocumentViewModelFreeText(text);
    if(strcmp(name,"draft")==0)CHECK(after.dirty);
    OK(umi_fs_read_text(path,&text,&bytes));CHECK(strcmp(text,saved)==0);free(text);
    umi_document_coordinator_destroy(documents);umi_document_store_destroy(store);umi_ui_workbench_destroy(workbench);umi_command_registry_destroy(commands);
    CHECK(remove(path)==0);OK(umi_fs_remove_tree(root));return 0;
}
