/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workflow_tools/test_documents.c
 * PURPOSE:
 *   Verify source-tab cycling without altering drafts, saved text or history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/navigation.h"
#include "umicom/document/edit.h"
#include "umicom/document/local_provider.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define GETPID _getpid
#else
#include <unistd.h>
#define GETPID getpid
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while(0)

typedef struct Fixture {
    UmiCommandRegistry *commands; UmiUiWorkbench *workbench;
    UmiDocumentStore *store; UmiDocumentCoordinator *documents;
    UmiDocumentId ids[3]; char views[3][UMI_UI_ID_CAPACITY];
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY]; int ownsRoot;
} Fixture;
static int Setup(Fixture *f,const char *name,int count)
{
    char tmp[UMI_PATH_CAPACITY],leaf[128];
    CHECK(umi_fs_temp_directory(tmp,sizeof tmp)==UMI_STATUS_OK);
    CHECK(snprintf(leaf,sizeof leaf,"umicom-tab-cycle-%ld-%s",(long)GETPID(),name)>0);
    CHECK(umi_fs_join(f->root,sizeof f->root,tmp,leaf)==UMI_STATUS_OK);
    CHECK(!umi_fs_exists(f->root));
    CHECK(umi_fs_make_directories(f->root)==UMI_STATUS_OK); f->ownsRoot=1;
    CHECK(umi_fs_join(f->path,sizeof f->path,f->root,"notes.c")==UMI_STATUS_OK);
    CHECK(umi_fs_write_text(f->path,"int notes = 1;\n")==UMI_STATUS_OK);
    CHECK(umi_command_registry_create(&f->commands)==UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("workflow.tabs",f->commands,&f->workbench)==UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store)==UMI_STATUS_OK);
    UmiDocumentProvider provider=umi_document_local_provider();
    CHECK(umi_document_coordinator_create(f->store,f->workbench,&provider,&f->documents)==UMI_STATUS_OK);
    for(int i=0;i<count;++i) {
        UmiStatus status=i==0?umi_document_coordinator_open(f->documents,f->path,f->views[i],sizeof f->views[i]):
            umi_document_coordinator_new(f->documents,i==1?"notes.h":"tests.c",f->views[i],sizeof f->views[i]);
        CHECK(status==UMI_STATUS_OK);
        UmiDocumentWorkingCopySnapshot snapshot;
        CHECK(umi_document_coordinator_active_snapshot(f->documents,&snapshot)==UMI_STATUS_OK);
        f->ids[i]=snapshot.document_id;
    }
    return 0;
}
static void Destroy(Fixture *f) {
    umi_document_coordinator_destroy(f->documents);umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench);umi_command_registry_destroy(f->commands);
    if(f->ownsRoot)(void)umi_fs_remove_tree(f->root);
}
static int Active(Fixture *f,UmiDocumentId id) {
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(f->documents,&snapshot)==UMI_STATUS_OK);
    CHECK(snapshot.document_id==id);return 0;
}
static int Run(Fixture *f,const char *name) {
    UmiDocumentId output=999;
    if(strcmp(name,"empty")==0) {
        CHECK(UmiDocumentCoordinatorCycle(f->documents,1,&output)==UMI_STATUS_NOT_FOUND);
        CHECK(output==999);
    } else if(strcmp(name,"single")==0) {
        UmiDocumentWorkingCopySnapshot before,after;
        CHECK(umi_document_coordinator_active_snapshot(f->documents,&before)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorCycle(f->documents,1,&output)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorCycle(f->documents,-1,NULL)==UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(f->documents,&after)==UMI_STATUS_OK);
        CHECK(output==f->ids[0] && before.revision==after.revision);
    } else if(strcmp(name,"invalid")==0) {
        CHECK(UmiDocumentCoordinatorCycle(NULL,1,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorCycle(f->documents,0,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorCycle(f->documents,2,&output)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(output==999); CHECK(Active(f,f->ids[2])==0);
    } else if(strcmp(name,"forward")==0) {
        for(size_t i=0;i<3;++i) {
            CHECK(UmiDocumentCoordinatorCycle(f->documents,1,&output)==UMI_STATUS_OK);
            CHECK(output==f->ids[i]);CHECK(Active(f,output)==0);
        }
    } else if(strcmp(name,"backward")==0) {
        const size_t order[]={1,0,2};
        for(size_t i=0;i<3;++i) {
            CHECK(UmiDocumentCoordinatorCycle(f->documents,-1,&output)==UMI_STATUS_OK);
            CHECK(output==f->ids[order[i]]);CHECK(Active(f,output)==0);
        }
    } else if(strcmp(name,"closed")==0) {
        CHECK(UmiDocumentCoordinatorClose(f->documents,f->ids[1],1)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorCycle(f->documents,-1,&output)==UMI_STATUS_OK);
        CHECK(output==f->ids[0]);
        CHECK(UmiDocumentCoordinatorCycle(f->documents,1,&output)==UMI_STATUS_OK);
        CHECK(output==f->ids[2]);CHECK(umi_document_coordinator_count(f->documents)==2);
    } else if(strcmp(name,"drafts")==0 || strcmp(name,"readonly")==0) {
        UmiUiDocumentViewModel *views=umi_ui_workbench_documents(f->workbench);
        UmiUiDocumentViewSnapshot before,after;
        CHECK(umi_ui_document_view_model_find(views,f->views[0],&before)==UMI_STATUS_OK);
        const char *draft="int notes = 8; /* draft */\n";
        CHECK(UmiUiDocumentViewModelUpsertText(views,&before,draft,strlen(draft))==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorSyncDocument(f->documents,f->ids[0])==UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views,f->views[0],&before)==UMI_STATUS_OK);
        before.cursor_offset=4;before.selection_length=5;before.pinned=1;
        if(strcmp(name,"readonly")==0)before.read_only=1;
        CHECK(umi_ui_document_view_model_upsert(views,&before)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorCycle(f->documents,1,&output)==UMI_STATUS_OK);CHECK(output==f->ids[0]);
        CHECK(UmiDocumentCoordinatorCycle(f->documents,1,NULL)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorCycle(f->documents,-1,NULL)==UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views,f->views[0],&after)==UMI_STATUS_OK);
        CHECK(after.cursor_offset==4 && after.selection_length==5 && after.read_only==before.read_only && after.pinned);
        char *text=NULL;size_t bytes=0;
        CHECK(UmiUiDocumentViewModelCopyText(views,f->views[0],&text,&bytes)==UMI_STATUS_OK);
        CHECK(bytes==strlen(draft)&&memcmp(text,draft,bytes)==0);UmiUiDocumentViewModelFreeText(text);
        CHECK(umi_fs_read_text(f->path,&text,&bytes)==UMI_STATUS_OK);
        CHECK(strcmp(text,"int notes = 1;\n")==0);free(text);
        if(!after.read_only) {
            CHECK(UmiDocumentCoordinatorUndo(f->documents,f->ids[0])==UMI_STATUS_OK);
            CHECK(UmiUiDocumentViewModelCopyText(views,f->views[0],&text,&bytes)==UMI_STATUS_OK);
            CHECK(strcmp(text,"int notes = 1;\n")==0);UmiUiDocumentViewModelFreeText(text);
        }
    } else return 2;
    return 0;
}
int main(int argc,char **argv) {
    if(argc!=2)return 2;
    Fixture f={0};int count=strcmp(argv[1],"empty")==0?0:strcmp(argv[1],"single")==0?1:3;
    int result=Setup(&f,argv[1],count);if(!result)result=Run(&f,argv[1]);Destroy(&f);return result;
}
