/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_closing/test_close_plan.c
 * PURPOSE: Exercise reviewed closing with real documents, history and files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/close.h"
#include "umicom/document/edit.h"
#include "umicom/document/local_provider.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define PROCESS_ID _getpid
#else
#include <unistd.h>
#define PROCESS_ID getpid
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiDocumentClosePlan *plan;
    char root[UMI_PATH_CAPACITY];
    char path[3][UMI_PATH_CAPACITY];
    char view[3][UMI_UI_ID_CAPACITY];
    UmiDocumentId id[3];
    int madeRoot;
} Fixture;
static int Start(Fixture *f, const char *name)
{
    char temp[UMI_PATH_CAPACITY], leaf[160];
    CHECK(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    CHECK(snprintf(leaf, sizeof(leaf), "umicom-close-plan-%ld-%s", (long)PROCESS_ID(), name) > 0);
    CHECK(umi_fs_join(f->root, sizeof(f->root), temp, leaf) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(f->root));
    CHECK(umi_fs_make_directories(f->root) == UMI_STATUS_OK); f->madeRoot = 1;
    CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.document.closing", f->commands, &f->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
    UmiDocumentProvider provider = umi_document_local_provider();
    CHECK(umi_document_coordinator_create(f->store, f->workbench, &provider, &f->documents) == UMI_STATUS_OK);
    return 0;
}
static void Stop(Fixture *f)
{
    UmiDocumentClosePlanDestroy(f->plan);
    umi_document_coordinator_destroy(f->documents);
    umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench);
    umi_command_registry_destroy(f->commands);
    if (f->madeRoot) (void)umi_fs_remove_tree(f->root);
}
static int Add(Fixture *f, size_t slot, int named)
{
    char leaf[64];
    CHECK(snprintf(leaf, sizeof(leaf), "notes-%zu.c", slot) > 0);
    CHECK(umi_fs_join(f->path[slot], sizeof(f->path[slot]), f->root, leaf) == UMI_STATUS_OK);
    if (named) {
        CHECK(umi_fs_write_text(f->path[slot], "original\n") == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_open(f->documents, f->path[slot], f->view[slot], sizeof(f->view[slot])) == UMI_STATUS_OK);
    } else CHECK(umi_document_coordinator_new(f->documents, leaf, f->view[slot], sizeof(f->view[slot])) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &snapshot) == UMI_STATUS_OK);
    f->id[slot] = snapshot.document_id;
    return 0;
}
static int Edit(Fixture *f, size_t slot, const char *text)
{
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->view[slot], &view) == UMI_STATUS_OK);
    view.dirty = 1; view.cursor_offset = 0U; view.selection_length = 0U;
    CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &view, text, strlen(text)) == UMI_STATUS_OK);
    return 0;
}
static int Text(Fixture *f, size_t slot, const char *expected, int disk)
{
    char *text = NULL; size_t bytes = 0U;
    UmiStatus status = disk ? umi_fs_read_text(f->path[slot], &text, &bytes) :
        UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench), f->view[slot], &text, &bytes);
    CHECK(status == UMI_STATUS_OK);
    int same = bytes == strlen(expected) && memcmp(text, expected, bytes) == 0;
    if (disk) free(text); else UmiUiDocumentViewModelFreeText(text);
    CHECK(same); return 0;
}

static int Begin(Fixture *f, size_t slot)
{
    CHECK(UmiDocumentCoordinatorPrepareClose(f->documents, f->id[slot], &f->plan) == UMI_STATUS_OK);
    return 0;
}
static int Exists(Fixture *f, size_t slot)
{
    for (size_t i=0U; i<umi_document_coordinator_count(f->documents); ++i) {
        UmiDocumentWorkingCopySnapshot s;
        if (umi_document_coordinator_at(f->documents,i,&s) == UMI_STATUS_OK && s.document_id==f->id[slot]) return 1;
    }
    return 0;
}
static int ReadOnly(Fixture *f, size_t slot, int readonly)
{
    UmiUiDocumentViewSnapshot v;
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench),f->view[slot],&v)==UMI_STATUS_OK);
    v.read_only=readonly;
    CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench),&v)==UMI_STATUS_OK);
    return 0;
}
static int Run(Fixture *f,const char *name)
{
    if (strcmp(name,"invalid")==0) {
        UmiDocumentCloseSummary s={0};s.text_bytes=77;
        CHECK(UmiDocumentCoordinatorPrepareClose(NULL,1U,&f->plan)==UMI_STATUS_INVALID_ARGUMENT && f->plan==NULL);
        CHECK(UmiDocumentCoordinatorPrepareClose(f->documents,0U,&f->plan)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorPrepareClose(f->documents,1U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentClosePlanSummary(NULL,&s)==UMI_STATUS_INVALID_ARGUMENT && s.text_bytes==77);
        CHECK(UmiDocumentCoordinatorCheckClose(NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorApplyClose(NULL,NULL,UMI_DOCUMENT_CLOSE_CANCEL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        UmiDocumentClosePlanDestroy(NULL);return 0;
    }
    if (strcmp(name,"missing")==0) {
        CHECK(UmiDocumentCoordinatorPrepareClose(f->documents,999U,&f->plan)==UMI_STATUS_NOT_FOUND);
        CHECK(f->plan==NULL);return 0;
    }
    int untitled=strstr(name,"untitled")!=NULL || strcmp(name,"path-limits")==0 ||
        strcmp(name,"destination-owned")==0 || strcmp(name,"write-failure")==0;
    CHECK(Add(f,0U,!untitled)==0);
    int clean=strcmp(name,"clean")==0 || strcmp(name,"cancel-clean")==0 ||
        strcmp(name,"save-clean")==0 || strcmp(name,"untitled-empty")==0;
    if (!clean) CHECK(Edit(f,0U,"int saved_notes = 3;\n")==0);
    if (strcmp(name,"readonly-save")==0 || strcmp(name,"readonly-discard")==0)
        CHECK(ReadOnly(f,0U,1)==0);
    CHECK(Begin(f,0U)==0);
    CHECK(UmiDocumentCoordinatorCheckClose(f->documents,f->plan)==UMI_STATUS_OK);
    UmiDocumentCloseSummary summary;
    CHECK(UmiDocumentClosePlanSummary(f->plan,&summary)==UMI_STATUS_OK);
    CHECK(summary.document_id==f->id[0] && summary.has_path==!untitled);
    if (strcmp(name,"summary")==0) {
        CHECK(summary.dirty && summary.text_bytes==strlen("int saved_notes = 3;\n"));
        summary.dirty=0;summary.document_id=999;
        CHECK(UmiDocumentClosePlanSummary(f->plan,&summary)==UMI_STATUS_OK && summary.dirty && summary.document_id==f->id[0]);
        UmiDocumentWorkingCopySnapshot s;CHECK(umi_document_coordinator_at(f->documents,0,&s)==UMI_STATUS_OK);
        CHECK(s.undo_count==0U);CHECK(Text(f,0,"original\n",1)==0);return 0;
    }
    if (strcmp(name,"clean")==0 || strcmp(name,"save-clean")==0) {
        CHECK(!summary.dirty);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,
            strcmp(name,"clean")==0?UMI_DOCUMENT_CLOSE_UNMODIFIED:UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_OK);
        CHECK(!Exists(f,0));CHECK(Text(f,0,"original\n",1)==0);return 0;
    }
    if (strncmp(name,"cancel",6)==0) {
        if (strcmp(name,"cancel-stale")==0) CHECK(Edit(f,0,"newer draft\n")==0);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_CANCEL,NULL)==UMI_STATUS_CANCELLED);
        CHECK(Exists(f,0));CHECK(Text(f,0,"original\n",1)==0);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_DISCARD,NULL)==UMI_STATUS_INVALID_STATE);
        return 0;
    }
    if (strcmp(name,"dirty-refused")==0 || strcmp(name,"untitled-empty")==0) {
        CHECK(summary.dirty);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_UNMODIFIED,NULL)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentCoordinatorCheckClose(f->documents,f->plan)==UMI_STATUS_OK);
        CHECK(Exists(f,0));return 0;
    }
    if (strcmp(name,"discard")==0 || strcmp(name,"readonly-discard")==0 || strcmp(name,"untitled-discard")==0) {
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_DISCARD,NULL)==UMI_STATUS_OK);
        CHECK(!Exists(f,0));
        CHECK(umi_ui_document_view_model_count(umi_ui_workbench_documents(f->workbench))==0U);
        CHECK(umi_document_coordinator_count(f->documents)==0U);
        if (!untitled) CHECK(Text(f,0,"original\n",1)==0);else CHECK(!umi_fs_exists(f->path[0]));
        return 0;
    }
    if (strcmp(name,"save")==0 || strcmp(name,"untitled-save")==0) {
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,untitled?f->path[0]:NULL)==UMI_STATUS_OK);
        CHECK(!Exists(f,0));CHECK(Text(f,0,"int saved_notes = 3;\n",1)==0);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentClosePlanSummary(f->plan,&summary)==UMI_STATUS_INVALID_STATE);return 0;
    }
    if (strcmp(name,"readonly-save")==0) {
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_PERMISSION_DENIED);
        CHECK(Exists(f,0));CHECK(Text(f,0,"original\n",1)==0);return 0;
    }
    if (strcmp(name,"path-limits")==0) {
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,"")==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,"notes.h")==UMI_STATUS_INVALID_ARGUMENT);
        char path[UMI_PATH_CAPACITY];memset(path,'p',sizeof(path));
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,path)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiDocumentCoordinatorCheckClose(f->documents,f->plan)==UMI_STATUS_OK);return 0;
    }
    if (strcmp(name,"invalid-decision")==0) {
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,(UmiDocumentCloseDecision)0,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,(UmiDocumentCloseDecision)99,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_DISCARD,f->path[0])==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,f->path[0])==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Exists(f,0));return 0;
    }
    if (strcmp(name,"changed-text")==0 || strcmp(name,"history-cycle")==0 || strcmp(name,"independent-save")==0 ||
        strcmp(name,"readonly-change")==0 || strcmp(name,"renamed")==0) {
        if (strcmp(name,"changed-text")==0) CHECK(Edit(f,0,"newer draft\n")==0);
        if (strcmp(name,"history-cycle")==0) {
            CHECK(UmiDocumentCoordinatorSyncDocument(f->documents,f->id[0])==UMI_STATUS_OK);
            CHECK(UmiDocumentCoordinatorUndo(f->documents,f->id[0])==UMI_STATUS_OK);
            CHECK(UmiDocumentCoordinatorRedo(f->documents,f->id[0])==UMI_STATUS_OK);
        }
        if (strcmp(name,"independent-save")==0) CHECK(UmiDocumentCoordinatorSaveAs(f->documents,f->id[0],f->path[0])==UMI_STATUS_OK);
        if (strcmp(name,"readonly-change")==0) CHECK(ReadOnly(f,0,1)==0);
        if (strcmp(name,"renamed")==0) {
            CHECK(umi_fs_join(f->path[2],sizeof(f->path[2]),f->root,"renamed.c")==UMI_STATUS_OK);
            CHECK(UmiDocumentCoordinatorSaveAs(f->documents,f->id[0],f->path[2])==UMI_STATUS_OK);
        }
        CHECK(UmiDocumentCoordinatorCheckClose(f->documents,f->plan)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_DISCARD,NULL)==UMI_STATUS_INVALID_STATE);
        CHECK(Exists(f,0));return 0;
    }
    if (strcmp(name,"closed")==0 || strcmp(name,"reopened")==0 || strcmp(name,"view-missing")==0) {
        if (strcmp(name,"view-missing")==0) CHECK(umi_ui_document_view_model_remove(umi_ui_workbench_documents(f->workbench),f->view[0])==UMI_STATUS_OK);
        else CHECK(UmiDocumentCoordinatorClose(f->documents,f->id[0],1)==UMI_STATUS_OK);
        if (strcmp(name,"reopened")==0) {
            char view[UMI_UI_ID_CAPACITY];CHECK(umi_document_coordinator_open(f->documents,f->path[0],view,sizeof(view))==UMI_STATUS_OK);
        }
        CHECK(UmiDocumentCoordinatorCheckClose(f->documents,f->plan)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_DISCARD,NULL)==UMI_STATUS_NOT_FOUND);
        CHECK(Text(f,0,"original\n",1)==0);return 0;
    }
    if (strcmp(name,"other-active")==0 || strcmp(name,"other-edits")==0 || strcmp(name,"selection")==0 || strcmp(name,"compacted")==0) {
        CHECK(Add(f,1U,1)==0);CHECK(Edit(f,1,"keep header draft\n")==0);
        if (strcmp(name,"selection")==0) {
            UmiUiDocumentViewSnapshot v;
            CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench),f->view[0],&v)==UMI_STATUS_OK);
            v.cursor_offset=4U;v.selection_length=3U;
            CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench),&v)==UMI_STATUS_OK);
        }
        if (strcmp(name,"compacted")==0) {
            UmiDocumentClosePlanDestroy(f->plan);f->plan=NULL;
            CHECK(Begin(f,1)==0);CHECK(UmiDocumentCoordinatorClose(f->documents,f->id[0],1)==UMI_STATUS_OK);
            CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_OK);
            CHECK(Text(f,1,"keep header draft\n",1)==0);return 0;
        }
        CHECK(UmiDocumentCoordinatorCheckClose(f->documents,f->plan)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_OK);
        UmiDocumentWorkingCopySnapshot active;CHECK(umi_document_coordinator_active_snapshot(f->documents,&active)==UMI_STATUS_OK);
        CHECK(active.document_id==f->id[1] && active.dirty);
        CHECK(Text(f,1,"keep header draft\n",0)==0);CHECK(Text(f,1,"original\n",1)==0);return 0;
    }
    if (strcmp(name,"external-conflict")==0) {
        CHECK(umi_fs_write_text(f->path[0],"external content\n")==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_INVALID_STATE);
        CHECK(Exists(f,0));CHECK(Text(f,0,"int saved_notes = 3;\n",0)==0);
        CHECK(Text(f,0,"external content\n",1)==0);return 0;
    }
    if (strcmp(name,"destination-owned")==0 || strcmp(name,"write-failure")==0) {
        const char *dest=f->path[1];
        if (strcmp(name,"destination-owned")==0) CHECK(Add(f,1U,1)==0);
        else dest=f->root; /* A directory itself cannot be a source file. */
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,dest)!=UMI_STATUS_OK);
        CHECK(Exists(f,0));CHECK(Text(f,0,"int saved_notes = 3;\n",0)==0);return 0;
    }
    if (strcmp(name,"destroy-owner")==0) {
        umi_document_coordinator_destroy(f->documents);f->documents=NULL;
        UmiDocumentClosePlanDestroy(f->plan);f->plan=NULL;return 0;
    }
    if (strcmp(name,"large")==0) {
        UmiDocumentClosePlanDestroy(f->plan);f->plan=NULL;
        size_t n=128U*1024U;char *text=malloc(n+1U);CHECK(text!=NULL);
        memset(text,'n',n);text[n]='\0';int result=Edit(f,0,text);free(text);CHECK(result==0);
        CHECK(Begin(f,0)==0);CHECK(UmiDocumentClosePlanSummary(f->plan,&summary)==UMI_STATUS_OK && summary.text_bytes==n);
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents,f->plan,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_OK);
        char *disk=NULL;size_t bytes=0U;CHECK(umi_fs_read_text(f->path[0],&disk,&bytes)==UMI_STATUS_OK);
        int same=bytes==n && disk[0]=='n' && disk[n-1U]=='n';free(disk);CHECK(same);return 0;
    }
    fprintf(stderr,"unknown case: %s\n",name);return 2;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    Fixture f={0};int result=Start(&f,argv[1]);
    if(result==0)result=Run(&f,argv[1]);
    Stop(&f);if(result==0)printf("PASS %s\n",argv[1]);return result;
}
