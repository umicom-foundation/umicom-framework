/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_closing/test_close_session.c
 * PURPOSE:
 *   Exercise multi-document closing with the production coordinator and real
 *   temporary files. Every check remains active in optimised builds.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/close_session.h"
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
    UmiDocumentCloseSession *session;
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
    CHECK(snprintf(leaf, sizeof(leaf), "umicom-close-sequence-%ld-%s", (long)PROCESS_ID(), name) > 0);
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
    UmiDocumentCloseSessionDestroy(f->session);
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
static int Sequence(Fixture *f, UmiDocumentCloseScope scope, UmiDocumentId keep)
{
    CHECK(UmiDocumentCloseSessionCreate(f->documents, scope, keep, &f->session) == UMI_STATUS_OK);
    return 0;
}
static int Progress(Fixture *f, UmiDocumentClosePhase phase, size_t closed,
    size_t alreadyClosed, size_t remaining)
{
    UmiDocumentCloseProgress p;
    CHECK(UmiDocumentCloseSessionProgress(f->session, &p) == UMI_STATUS_OK);
    CHECK(p.phase == phase && p.closed == closed && p.already_closed == alreadyClosed && p.remaining == remaining);
    CHECK(p.total == closed + alreadyClosed + remaining);
    char text[1024];
    CHECK(UmiDocumentCloseProgressFormat(&p, text, sizeof(text)) == UMI_STATUS_OK);
    return 0;
}
static int Run(Fixture *f, const char *name)
{
    if (strcmp(name,"invalid")==0) {
        UmiDocumentCloseProgress p={0}; p.total=77U;
        CHECK(UmiDocumentCloseSessionCreate(NULL,UMI_DOCUMENT_CLOSE_ALL,0U,&f->session)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(f->session==NULL);
        CHECK(UmiDocumentCloseSessionCreate(f->documents,(UmiDocumentCloseScope)99,0U,&f->session)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionCreate(f->documents,UMI_DOCUMENT_CLOSE_ALL,1U,&f->session)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionCreate(f->documents,UMI_DOCUMENT_CLOSE_OTHERS,0U,&f->session)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionCreate(f->documents,UMI_DOCUMENT_CLOSE_ALL,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionCreate(f->documents,UMI_DOCUMENT_CLOSE_OTHERS,1U,&f->session)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiDocumentCloseSessionStep(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionRespond(NULL,UMI_DOCUMENT_CLOSE_CANCEL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionCancel(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionCheck(NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionProgress(NULL,&p)==UMI_STATUS_INVALID_ARGUMENT && p.total==77U);
        UmiDocumentCloseSessionDestroy(NULL);return 0;
    }
    if (strcmp(name,"empty")==0) {
        CHECK(Sequence(f,UMI_DOCUMENT_CLOSE_ALL,0U)==0);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_COMPLETE,0U,0U,0U)==0);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionCancel(f->session)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_INVALID_STATE);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_COMPLETE,0U,0U,0U)==0);return 0;
    }
    if (strcmp(name,"format-status")==0) {
        UmiDocumentCloseProgress p={0};p.phase=UMI_DOCUMENT_CLOSE_FAILED;p.last_status=(UmiStatus)99;
        p.total=1U;p.remaining=1U;char text[128]="unchanged";
        CHECK(UmiDocumentCloseProgressFormat(&p,text,sizeof(text))==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(text,"unchanged")==0);return 0;
    }
    if (strncmp(name,"format-",7)==0) {
        UmiDocumentCloseProgress p={0};p.phase=UMI_DOCUMENT_CLOSE_READY;p.last_status=UMI_STATUS_OK;
        p.total=3U;p.closed=1U;p.already_closed=1U;p.remaining=1U;
        memcpy(p.current.display_name,"notes.c",8U);
        char text[1024]="unchanged"; UmiDocumentCloseProgress before=p;
        if(strcmp(name,"format-counts")==0) {
            CHECK(UmiDocumentCloseProgressFormat(&p,text,sizeof(text))==UMI_STATUS_OK);
            CHECK(strstr(text,"1 closed, 1 already closed, 1 remaining")!=NULL);
            CHECK(memcmp(&p,&before,sizeof(p))==0);
            p.closed=SIZE_MAX;strcpy(text,"unchanged");
            CHECK(UmiDocumentCloseProgressFormat(&p,text,sizeof(text))==UMI_STATUS_INVALID_ARGUMENT);
        } else if(strcmp(name,"format-capacity")==0) {
            CHECK(UmiDocumentCloseProgressFormat(&p,text,1U)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(UmiDocumentCloseProgressFormat(NULL,text,sizeof(text))==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDocumentCloseProgressFormat(&p,NULL,sizeof(text))==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDocumentCloseProgressFormat(&p,text,0U)==UMI_STATUS_INVALID_ARGUMENT);
        } else if(strcmp(name,"format-name")==0) {
            memset(p.current.display_name,'x',sizeof(p.current.display_name));
            CHECK(UmiDocumentCloseProgressFormat(&p,text,sizeof(text))==UMI_STATUS_INVALID_ARGUMENT);
        } else if(strcmp(name,"format-phases")==0) {
            for(int phase=UMI_DOCUMENT_CLOSE_READY;phase<=UMI_DOCUMENT_CLOSE_CANCELLED;++phase) {
                p=before;p.phase=(UmiDocumentClosePhase)phase;
                if(p.phase==UMI_DOCUMENT_CLOSE_COMPLETE){p.total=2U;p.remaining=0U;}
                if(p.phase==UMI_DOCUMENT_CLOSE_FAILED)p.last_status=UMI_STATUS_IO_ERROR;
                if(p.phase==UMI_DOCUMENT_CLOSE_CANCELLED)p.last_status=UMI_STATUS_CANCELLED;
                CHECK(UmiDocumentCloseProgressFormat(&p,text,sizeof(text))==UMI_STATUS_OK);
            }
            p=before;p.phase=UMI_DOCUMENT_CLOSE_COMPLETE;strcpy(text,"unchanged");
            CHECK(UmiDocumentCloseProgressFormat(&p,text,sizeof(text))==UMI_STATUS_INVALID_ARGUMENT);
        } else return 2;
        CHECK(strcmp(text,"unchanged")==0);return 0;
    }
    if (strcmp(name,"activation-guards")==0) {
        CHECK(Add(f,0U,1)==0);CHECK(Add(f,1U,1)==0);
        UmiUiWorkbenchSnapshot before,after;
        CHECK(umi_ui_workbench_snapshot(f->workbench,&before)==UMI_STATUS_OK);
        CHECK(UmiUiWorkbenchClearClosedDocument(f->workbench,f->view[1])==UMI_STATUS_INVALID_STATE);
        CHECK(UmiUiWorkbenchClearClosedDocument(f->workbench,"absent-source")==UMI_STATUS_OK);
        CHECK(umi_ui_workbench_snapshot(f->workbench,&after)==UMI_STATUS_OK);
        CHECK(strcmp(before.active_document_view,after.active_document_view)==0 && before.revision==after.revision);
        CHECK(UmiUiWorkbenchClearClosedDocument(NULL,"x")==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiWorkbenchClearClosedDocument(f->workbench,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiUiWorkbenchClearClosedDocument(f->workbench,"")==UMI_STATUS_INVALID_ARGUMENT);
        char huge[UMI_UI_ID_CAPACITY];memset(huge,'x',sizeof(huge));
        CHECK(UmiUiWorkbenchClearClosedDocument(f->workbench,huge)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiDocumentCoordinatorClose(f->documents,f->id[0],0)==UMI_STATUS_OK);
        CHECK(umi_ui_workbench_snapshot(f->workbench,&after)==UMI_STATUS_OK);
        CHECK(strcmp(after.active_document_view,f->view[1])==0);
        CHECK(UmiDocumentCoordinatorClose(f->documents,f->id[1],0)==UMI_STATUS_OK);
        CHECK(umi_ui_workbench_snapshot(f->workbench,&before)==UMI_STATUS_OK && before.active_document_view[0]=='\0');
        CHECK(UmiUiWorkbenchClearClosedDocument(f->workbench,f->view[1])==UMI_STATUS_OK);
        CHECK(umi_ui_workbench_snapshot(f->workbench,&after)==UMI_STATUS_OK && before.revision==after.revision);
        return 0;
    }
    if (strcmp(name,"untitled-save")==0 || strcmp(name,"path-cancel")==0 ||
        strcmp(name,"path-stale")==0 || strcmp(name,"bad-path")==0) {
        CHECK(Add(f,0U,0)==0);CHECK(Edit(f,0U,"#define NOTES_CAPACITY 16\n")==0);
        CHECK(Sequence(f,UMI_DOCUMENT_CLOSE_ALL,0U)==0);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_DECISION,0U,0U,1U)==0);
        CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_OK);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_PATH,0U,0U,1U)==0);
        if(strcmp(name,"path-cancel")==0) {
            CHECK(UmiDocumentCloseSessionCancel(f->session)==UMI_STATUS_OK);
            CHECK(Exists(f,0U));CHECK(Progress(f,UMI_DOCUMENT_CLOSE_CANCELLED,0U,0U,1U)==0);return 0;
        }
        if(strcmp(name,"bad-path")==0) {
            char path[UMI_PATH_CAPACITY];memset(path,'x',sizeof(path));
            CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_SAVE,"relative.h")==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_SAVE,"")==UMI_STATUS_INVALID_ARGUMENT);
            CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_SAVE,path)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(Progress(f,UMI_DOCUMENT_CLOSE_PATH,0U,0U,1U)==0);return 0;
        }
        CHECK(umi_fs_join(f->path[0],sizeof(f->path[0]),f->root,"notes.h")==UMI_STATUS_OK);
        if(strcmp(name,"path-stale")==0) {
            CHECK(Edit(f,0U,"#define NOTES_CAPACITY 32\n")==0);
            CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_SAVE,f->path[0])==UMI_STATUS_INVALID_STATE);
            CHECK(!umi_fs_exists(f->path[0]) && Exists(f,0U));return 0;
        }
        CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_SAVE,f->path[0])==UMI_STATUS_OK);
        CHECK(!Exists(f,0U));CHECK(Text(f,0U,"#define NOTES_CAPACITY 16\n",1)==0);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_COMPLETE,1U,0U,0U)==0);return 0;
    }
    CHECK(Add(f,0U,1)==0);CHECK(Add(f,1U,1)==0);
    if(strcmp(name,"all-clean")==0) {
        CHECK(Sequence(f,UMI_DOCUMENT_CLOSE_ALL,0U)==0);
        CHECK(umi_document_coordinator_count(f->documents)==2U);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_READY,1U,0U,1U)==0);
        CHECK(!Exists(f,0U) && Exists(f,1U));
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_COMPLETE,2U,0U,0U)==0);
        UmiUiWorkbenchSnapshot final;
        CHECK(umi_ui_workbench_snapshot(f->workbench,&final)==UMI_STATUS_OK);
        CHECK(final.active_document_view[0]=='\0');return 0;
    }
    if(strcmp(name,"others")==0 || strcmp(name,"keep-switch")==0 || strcmp(name,"keep-closed")==0 || strcmp(name,"only-kept")==0) {
        if(strcmp(name,"only-kept")==0)CHECK(UmiDocumentCoordinatorClose(f->documents,f->id[1],0)==UMI_STATUS_OK);
        CHECK(Edit(f,0U,"kept draft\n")==0);
        CHECK(Sequence(f,UMI_DOCUMENT_CLOSE_OTHERS,f->id[0])==0);
        if(strcmp(name,"keep-switch")==0) CHECK(umi_ui_workbench_activate_document(f->workbench,f->view[1])==UMI_STATUS_OK);
        if(strcmp(name,"keep-closed")==0)CHECK(UmiDocumentCoordinatorClose(f->documents,f->id[0],1)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_COMPLETE,strcmp(name,"only-kept")==0?0U:1U,0U,0U)==0);
        if(strcmp(name,"keep-closed")!=0){CHECK(Exists(f,0U));CHECK(Text(f,0U,"kept draft\n",0)==0);}
        return 0;
    }
    if(strcmp(name,"keep-missing")==0) {
        CHECK(UmiDocumentCloseSessionCreate(f->documents,UMI_DOCUMENT_CLOSE_OTHERS,UINT64_MAX,&f->session)==UMI_STATUS_NOT_FOUND);
        CHECK(f->session==NULL && umi_document_coordinator_count(f->documents)==2U);return 0;
    }
    CHECK(Edit(f,1U,"changed header\n")==0);
    if(strcmp(name,"new-document")==0) {
        CHECK(Sequence(f,UMI_DOCUMENT_CLOSE_ALL,0U)==0);CHECK(Add(f,2U,0)==0);
    } else CHECK(Sequence(f,UMI_DOCUMENT_CLOSE_ALL,0U)==0);
    if(strcmp(name,"missing-view")==0) {
        CHECK(umi_ui_document_view_model_remove(umi_ui_workbench_documents(f->workbench),f->view[0])==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_NOT_FOUND);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_FAILED,0U,0U,2U)==0);
        CHECK(umi_document_coordinator_count(f->documents)==2U);return 0;
    }
    if(strcmp(name,"readonly-save")==0 || strcmp(name,"readonly-discard")==0) {
        CHECK(ReadOnly(f,1U,1)==0);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        int saving=strcmp(name,"readonly-save")==0;
        CHECK(UmiDocumentCloseSessionRespond(f->session,saving?UMI_DOCUMENT_CLOSE_SAVE:UMI_DOCUMENT_CLOSE_DISCARD,NULL)==
            (saving?UMI_STATUS_PERMISSION_DENIED:UMI_STATUS_OK));
        CHECK(Exists(f,1U)==saving);CHECK(Text(f,1U,"original\n",1)==0);return 0;
    }
    if(strcmp(name,"saved-before-turn")==0) {
        CHECK(UmiDocumentCoordinatorSaveAs(f->documents,f->id[1],f->path[1])==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_COMPLETE,2U,0U,0U)==0);
        CHECK(Text(f,1U,"changed header\n",1)==0);return 0;
    }
    if(strcmp(name,"cancel-before")==0 || strcmp(name,"destroy-session")==0 || strcmp(name,"destroy-owner")==0) {
        if(strcmp(name,"cancel-before")==0) {
            CHECK(UmiDocumentCloseSessionCancel(f->session)==UMI_STATUS_OK);
            CHECK(UmiDocumentCloseSessionCancel(f->session)==UMI_STATUS_OK);
            CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_CANCELLED);
            CHECK(Progress(f,UMI_DOCUMENT_CLOSE_CANCELLED,0U,0U,2U)==0);
        } else if(strcmp(name,"destroy-session")==0) {
            UmiDocumentCloseSessionDestroy(f->session);f->session=NULL;
        } else {
            umi_document_coordinator_destroy(f->documents);f->documents=NULL;
            UmiDocumentCloseSessionDestroy(f->session);f->session=NULL;return 0;
        }
        CHECK(Exists(f,0U)&&Exists(f,1U));CHECK(Text(f,1U,"changed header\n",0)==0);return 0;
    }
    if(strcmp(name,"separate-close")==0 || strcmp(name,"reopened")==0) {
        CHECK(UmiDocumentCoordinatorClose(f->documents,f->id[0],0)==UMI_STATUS_OK);
        if(strcmp(name,"reopened")==0) {
            CHECK(umi_document_coordinator_open(f->documents,f->path[0],f->view[2],sizeof(f->view[2]))==UMI_STATUS_OK);
        }
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_READY,0U,1U,1U)==0);
        return 0;
    }
    if(strcmp(name,"renamed-before")==0) {
        char path[UMI_PATH_CAPACITY];CHECK(umi_fs_join(path,sizeof(path),f->root,"renamed.c")==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorSaveAs(f->documents,f->id[0],path)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_INVALID_STATE);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_FAILED,0U,0U,2U)==0);CHECK(Exists(f,0U)&&Exists(f,1U));return 0;
    }
    CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
    CHECK(Progress(f,UMI_DOCUMENT_CLOSE_READY,1U,0U,1U)==0);
    if(strcmp(name,"cancel-between")==0) {
        CHECK(UmiDocumentCloseSessionCancel(f->session)==UMI_STATUS_OK);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_CANCELLED,1U,0U,1U)==0);CHECK(Exists(f,1U));return 0;
    }
    if(strcmp(name,"latest-before")==0) CHECK(Edit(f,1U,"latest header\n")==0);
    CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
    CHECK(Progress(f,UMI_DOCUMENT_CLOSE_DECISION,1U,0U,1U)==0);
    CHECK(UmiDocumentCloseSessionCheck(f->session)==UMI_STATUS_OK);
    if(strcmp(name,"repeated-step")==0) {
        UmiDocumentCloseProgress before,after;
        CHECK(UmiDocumentCloseSessionProgress(f->session,&before)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
        CHECK(UmiDocumentCloseSessionProgress(f->session,&after)==UMI_STATUS_OK);
        CHECK(memcmp(&before,&after,sizeof(before))==0);return 0;
    }
    if(strcmp(name,"cancel-question")==0 || strcmp(name,"retry")==0) {
        CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_CANCEL,NULL)==UMI_STATUS_CANCELLED);
        CHECK(Exists(f,1U)&&!Exists(f,0U));CHECK(Text(f,1U,"changed header\n",0)==0);
        if(strcmp(name,"retry")==0) {
            UmiDocumentCloseSessionDestroy(f->session);f->session=NULL;
            CHECK(Sequence(f,UMI_DOCUMENT_CLOSE_ALL,0U)==0);
            CHECK(Progress(f,UMI_DOCUMENT_CLOSE_READY,0U,0U,1U)==0);
            CHECK(UmiDocumentCloseSessionStep(f->session)==UMI_STATUS_OK);
            CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_DISCARD,NULL)==UMI_STATUS_OK);
            CHECK(Progress(f,UMI_DOCUMENT_CLOSE_COMPLETE,1U,0U,0U)==0);
        } return 0;
    }
    if(strcmp(name,"invalid-response")==0) {
        CHECK(UmiDocumentCloseSessionRespond(f->session,(UmiDocumentCloseDecision)99,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_DISCARD,"unused")==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCloseSessionRespond(f->session,UMI_DOCUMENT_CLOSE_SAVE,f->path[0])==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(Progress(f,UMI_DOCUMENT_CLOSE_DECISION,1U,0U,1U)==0);return 0;
    }
    UmiStatus expected=UMI_STATUS_OK;
    UmiDocumentCloseDecision decision=UMI_DOCUMENT_CLOSE_DISCARD;
    if(strcmp(name,"save")==0 || strcmp(name,"latest-before")==0 || strcmp(name,"external-conflict")==0 || strcmp(name,"write-failure")==0)
        decision=UMI_DOCUMENT_CLOSE_SAVE;
    if(strcmp(name,"stale")==0){CHECK(Edit(f,1U,"newer work\n")==0);expected=UMI_STATUS_INVALID_STATE;}
    if(strcmp(name,"saved-independently")==0){CHECK(UmiDocumentCoordinatorSaveAs(f->documents,f->id[1],f->path[1])==UMI_STATUS_OK);expected=UMI_STATUS_INVALID_STATE;}
    if(strcmp(name,"readonly-change")==0){CHECK(ReadOnly(f,1U,1)==0);expected=UMI_STATUS_INVALID_STATE;}
    if(strcmp(name,"closed-question")==0){CHECK(UmiDocumentCoordinatorClose(f->documents,f->id[1],1)==UMI_STATUS_OK);expected=UMI_STATUS_NOT_FOUND;}
    if(strcmp(name,"external-conflict")==0){CHECK(umi_fs_write_text(f->path[1],"external work\n")==UMI_STATUS_OK);expected=UMI_STATUS_INVALID_STATE;}
    if(strcmp(name,"write-failure")==0){CHECK(remove(f->path[1])==0);CHECK(umi_fs_make_directories(f->path[1])==UMI_STATUS_OK);expected=UMI_STATUS_IO_ERROR;}
    if(strcmp(name,"other-active")==0){CHECK(Add(f,2U,0)==0);CHECK(Edit(f,2U,"other draft\n")==0);}
    UmiStatus status=UmiDocumentCloseSessionRespond(f->session,decision,NULL);
    if(strcmp(name,"write-failure")==0)CHECK(status!=UMI_STATUS_OK);
    else CHECK(status==expected);
    CHECK(Progress(f,status==UMI_STATUS_OK?UMI_DOCUMENT_CLOSE_COMPLETE:UMI_DOCUMENT_CLOSE_FAILED,
        status==UMI_STATUS_OK?2U:1U,0U,status==UMI_STATUS_OK?0U:1U)==0);
    CHECK(UmiDocumentCloseSessionRespond(f->session,decision,NULL)==UMI_STATUS_INVALID_STATE);
    if(status!=UMI_STATUS_OK && strcmp(name,"closed-question")!=0)CHECK(Exists(f,1U));
    if(strcmp(name,"save")==0)CHECK(Text(f,1U,"changed header\n",1)==0);
    if(strcmp(name,"latest-before")==0)CHECK(Text(f,1U,"latest header\n",1)==0);
    if(strcmp(name,"new-document")==0 || strcmp(name,"other-active")==0)CHECK(Exists(f,2U));
    if(strcmp(name,"other-active")==0) {
        UmiDocumentWorkingCopySnapshot a;CHECK(umi_document_coordinator_active_snapshot(f->documents,&a)==UMI_STATUS_OK);
        CHECK(a.document_id==f->id[2]);CHECK(Text(f,2U,"other draft\n",0)==0);
    }
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    const char *cases[]={"activation-guards","format-status","keep-missing","missing-view","readonly-save","readonly-discard","saved-before-turn","invalid","empty","all-clean","others","keep-switch","keep-closed","only-kept","cancel-before","cancel-between","cancel-question","retry","destroy-session","destroy-owner","separate-close","reopened","renamed-before","repeated-step","invalid-response","stale","saved-independently","readonly-change","closed-question","external-conflict","write-failure","new-document","other-active","latest-before","save","discard","untitled-save","path-cancel","path-stale","bad-path","format-counts","format-capacity","format-name","format-phases"};
    int known=0;
    for(size_t index=0U;index<sizeof(cases)/sizeof(cases[0]);++index)
        if(strcmp(argv[1],cases[index])==0)known=1;
    if(!known)return 2;
    Fixture f={0};int result=Start(&f,argv[1]);
    if(result==0)result=Run(&f,argv[1]);
    Stop(&f);return result;
}
