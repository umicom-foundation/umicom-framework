/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_editing/test_document_editing.c
 * PURPOSE: Check editing, delayed paste targets and unchanged failure state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
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
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
#ifdef UMICOM_EDIT_ALLOCATION_TEST
/* Fail one real allocation after setup, then immediately disarm the fixture.
 * This is link-time fault injection, not a replacement document implementation. */
static long failAllocation = -1;
static int FailedAllocation(void)
{
    if (failAllocation < 0) return 0;
    if (failAllocation-- == 0) { failAllocation = -1; return 1; }
    return 0;
}
void *__real_malloc(size_t);
void *__real_calloc(size_t, size_t);
void *__real_realloc(void *, size_t);
void *__wrap_malloc(size_t bytes) { return FailedAllocation() ? NULL : __real_malloc(bytes); }
void *__wrap_calloc(size_t count, size_t bytes) { return FailedAllocation() ? NULL : __real_calloc(count, bytes); }
void *__wrap_realloc(void *value, size_t bytes) { return FailedAllocation() ? NULL : __real_realloc(value, bytes); }
#endif
#define ORIGINAL "Umicom Notes: saved\n"
typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiDocumentEditPlan *plan;
    UmiDocumentId id;
    char view[UMI_UI_ID_CAPACITY];
    char root[UMI_PATH_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    int madeRoot;
} Fixture;
static UmiUiDocumentViewModel *Views(Fixture *f) { return umi_ui_workbench_documents(f->workbench); }
static int Start(Fixture *f, const char *name)
{
    char temp[UMI_PATH_CAPACITY], leaf[128];
    CHECK(umi_fs_temp_directory(temp, sizeof temp) == UMI_STATUS_OK);
    CHECK(snprintf(leaf,sizeof leaf,"umicom-edit-%ld-%s",(long)PROCESS_ID(),name)>0);
    CHECK(umi_fs_join(f->root,sizeof f->root,temp,leaf)==UMI_STATUS_OK);
    CHECK(!umi_fs_exists(f->root));
    CHECK(umi_fs_make_directories(f->root)==UMI_STATUS_OK); f->madeRoot=1;
    CHECK(umi_fs_join(f->path,sizeof f->path,f->root,"notes.c")==UMI_STATUS_OK);
    CHECK(umi_fs_write_text(f->path,ORIGINAL)==UMI_STATUS_OK);
    CHECK(umi_command_registry_create(&f->commands)==UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.editing",f->commands,&f->workbench)==UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store)==UMI_STATUS_OK);
    UmiDocumentProvider provider=umi_document_local_provider();
    CHECK(umi_document_coordinator_create(f->store,f->workbench,&provider,&f->documents)==UMI_STATUS_OK);
    CHECK(umi_document_coordinator_open(f->documents,f->path,f->view,sizeof f->view)==UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(f->documents,&snapshot)==UMI_STATUS_OK);
    f->id=snapshot.document_id;
    return 0;
}
static void Stop(Fixture *f)
{
    UmiDocumentEditPlanDestroy(f->plan);
    umi_document_coordinator_destroy(f->documents);
    umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench);
    umi_command_registry_destroy(f->commands);
    if(f->madeRoot)(void)umi_fs_remove_tree(f->root);
}
static int View(Fixture *f, UmiUiDocumentViewSnapshot *out)
{ CHECK(umi_ui_document_view_model_find(Views(f),f->view,out)==UMI_STATUS_OK);return 0; }
static int Selection(Fixture *f,size_t offset,size_t count)
{
    UmiUiDocumentViewSnapshot view; CHECK(View(f,&view)==0);
    view.cursor_offset=offset;view.selection_length=count;
    CHECK(umi_ui_document_view_model_upsert(Views(f),&view)==UMI_STATUS_OK);return 0;
}
static int Edit(Fixture *f,const char *text,size_t length)
{
    UmiUiDocumentViewSnapshot view;CHECK(View(f,&view)==0);view.dirty=1;
    view.cursor_offset=0U;view.selection_length=0U;
    CHECK(UmiUiDocumentViewModelUpsertText(Views(f),&view,text,length)==UMI_STATUS_OK);return 0;
}
static int IsText(Fixture *f,const char *expected)
{
    char *text=NULL;size_t length=0U;
    CHECK(UmiUiDocumentViewModelCopyText(Views(f),f->view,&text,&length)==UMI_STATUS_OK);
    int equal=length==strlen(expected)&&memcmp(text,expected,length)==0;
    UmiUiDocumentViewModelFreeText(text); CHECK(equal);return 0;
}
static int DiskUnchanged(Fixture *f)
{
    char *text=NULL;size_t length=0U;CHECK(umi_fs_read_text(f->path,&text,&length)==UMI_STATUS_OK);
    int equal=length==strlen(ORIGINAL)&&memcmp(text,ORIGINAL,length)==0;
    free(text);CHECK(equal);return 0;
}
static int Prepare(Fixture *f)
{
    UmiDocumentEditPlanDestroy(f->plan);f->plan=NULL;
    CHECK(UmiDocumentCoordinatorPrepareEdit(f->documents,f->id,&f->plan)==UMI_STATUS_OK);return 0;
}
static int ReadOnly(Fixture *f,int value)
{
    UmiUiDocumentViewSnapshot view;CHECK(View(f,&view)==0);view.read_only=value;
    CHECK(umi_ui_document_view_model_upsert(Views(f),&view)==UMI_STATUS_OK);return 0;
}
static int Another(Fixture *f,char *view)
{
    CHECK(umi_document_coordinator_new(f->documents,"second-note.c",view,UMI_UI_ID_CAPACITY)==UMI_STATUS_OK);
    UmiUiDocumentViewSnapshot snapshot;CHECK(umi_ui_document_view_model_find(Views(f),view,&snapshot)==UMI_STATUS_OK);
    snapshot.dirty=1;CHECK(UmiUiDocumentViewModelUpsertText(Views(f),&snapshot,"Other unsaved note",18U)==UMI_STATUS_OK);
    return 0;
}
static int OtherUnchanged(Fixture *f,const char *view)
{
    UmiDocumentWorkingCopySnapshot active;CHECK(umi_document_coordinator_active_snapshot(f->documents,&active)==UMI_STATUS_OK);
    CHECK(strcmp(active.view_id,view)==0);
    char *copy=NULL;size_t length=0U;CHECK(UmiUiDocumentViewModelCopyText(Views(f),view,&copy,&length)==UMI_STATUS_OK);
    int same=length==18U&&memcmp(copy,"Other unsaved note",18U)==0;
    UmiUiDocumentViewModelFreeText(copy);CHECK(same);return 0;
}
static int Run(Fixture *f,const char *name)
{
    UmiDocumentEditState state;
    if(strcmp(name,"command-identities")==0){
        const char *ids[]={"edit.undo","edit.redo","edit.cut","edit.copy","edit.paste","edit.select-all","edit.delete"};
        for(size_t i=0;i<7U;++i){UmiDocumentEditCommand command=0;CHECK(UmiDocumentEditCommandFromId(ids[i],&command)==UMI_STATUS_OK);CHECK((size_t)command==i+1U);}
        UmiDocumentEditCommand command=UMI_DOCUMENT_EDIT_COPY;
        CHECK(UmiDocumentEditCommandFromId("edit.unknown",&command)==UMI_STATUS_NOT_FOUND);CHECK(command==UMI_DOCUMENT_EDIT_COPY);return 0;
    }
    if(strcmp(name,"invalid-input")==0){
        CHECK(UmiDocumentCoordinatorPrepareEdit(NULL,f->id,&f->plan)==UMI_STATUS_INVALID_ARGUMENT);CHECK(f->plan==NULL);
        CHECK(UmiDocumentCoordinatorPrepareEdit(f->documents,0U,&f->plan)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorUndo(NULL,f->id)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorRedo(f->documents,0U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorSelectAll(f->documents,UINT64_MAX)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiDocumentCoordinatorSyncDocument(f->documents,UINT64_MAX)==UMI_STATUS_NOT_FOUND);
        const char *p="unchanged";size_t n=8U;CHECK(UmiDocumentEditPlanSelection(NULL,&p,&n)==UMI_STATUS_INVALID_ARGUMENT);CHECK(p==NULL&&n==0U);return 0;
    }
    if(strcmp(name,"initial-state")==0){
        UmiDocumentSnapshot before,after;CHECK(umi_document_store_snapshot(f->store,f->id,&before)==UMI_STATUS_OK);
        uint64_t revision=umi_ui_document_view_model_revision(Views(f));
        CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&state)==UMI_STATUS_OK);
        CHECK(!state.can_undo&&!state.can_redo&&!state.read_only&&state.text_bytes==strlen(ORIGINAL));
        CHECK(umi_ui_document_view_model_revision(Views(f))==revision);
        CHECK(umi_document_store_snapshot(f->store,f->id,&after)==UMI_STATUS_OK);CHECK(before.revision==after.revision);return 0;
    }
    if(strcmp(name,"select-all")==0||strcmp(name,"readonly-selection")==0){
        if(strcmp(name,"readonly-selection")==0)CHECK(ReadOnly(f,1)==0);
        CHECK(UmiDocumentCoordinatorSelectAll(f->documents,f->id)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&state)==UMI_STATUS_OK);
        CHECK(state.selection_offset==0U&&state.selection_bytes==strlen(ORIGINAL));CHECK(!state.can_undo);
        CHECK(DiskUnchanged(f)==0);return 0;
    }
    if(strcmp(name,"pending-undo")==0||strcmp(name,"sync-group")==0||strcmp(name,"redo-new-edit")==0){
        CHECK(Edit(f,"Edited Notes",12U)==0);
        CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&state)==UMI_STATUS_OK);CHECK(state.can_undo&&!state.can_redo);
        if(strcmp(name,"sync-group")==0)CHECK(UmiDocumentCoordinatorSyncDocument(f->documents,f->id)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorUndo(f->documents,f->id)==UMI_STATUS_OK);CHECK(IsText(f,ORIGINAL)==0);
        CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&state)==UMI_STATUS_OK);CHECK(state.can_redo);
        if(strcmp(name,"redo-new-edit")==0){CHECK(Edit(f,"New branch",10U)==0);CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&state)==UMI_STATUS_OK);CHECK(!state.can_redo);}
        else {CHECK(UmiDocumentCoordinatorRedo(f->documents,f->id)==UMI_STATUS_OK);CHECK(IsText(f,"Edited Notes")==0);}
        CHECK(DiskUnchanged(f)==0);return 0;
    }
    if(strcmp(name,"large-tail")==0||strcmp(name,"maximum-size")==0){
        size_t bytes=strcmp(name,"maximum-size")==0?UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES:100000U;
        char *large=malloc(bytes+1U);CHECK(large!=NULL);memset(large,'x',bytes);large[bytes]='\0';
        CHECK(Edit(f,large,bytes)==0);CHECK(UmiDocumentCoordinatorSyncDocument(f->documents,f->id)==UMI_STATUS_OK);
        CHECK(Selection(f,bytes-1U,1U)==0);CHECK(Prepare(f)==0);
        if(bytes==UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES){CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"ab",2U)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(IsText(f,large)==0);}
        CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"N",1U)==UMI_STATUS_OK);large[bytes-1U]='N';CHECK(IsText(f,large)==0);
        CHECK(UmiDocumentCoordinatorUndo(f->documents,f->id)==UMI_STATUS_OK);large[bytes-1U]='x';CHECK(IsText(f,large)==0);free(large);return 0;
    }
    if(strcmp(name,"utf8-selection")==0||strcmp(name,"utf8-boundary")==0){
        const char *text="Notes \xc2\xa3" "5 \xf0\x9f\x93\x9d";
        CHECK(Edit(f,text,strlen(text))==0);
        CHECK(Selection(f,strcmp(name,"utf8-boundary")==0?7U:6U,2U)==0);
        if(strcmp(name,"utf8-boundary")==0){CHECK(UmiDocumentCoordinatorPrepareEdit(f->documents,f->id,&f->plan)==UMI_STATUS_INVALID_STATE);return 0;}
        CHECK(Prepare(f)==0);CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"GBP ",4U)==UMI_STATUS_OK);
        CHECK(IsText(f,"Notes GBP 5 \xf0\x9f\x93\x9d")==0);return 0;
    }
    if(strcmp(name,"utf8-history")==0){
        const char *oldText="\xc2\xa3" "5";
        CHECK(Edit(f,oldText,strlen(oldText))==0);
        CHECK(UmiDocumentCoordinatorSyncDocument(f->documents,f->id)==UMI_STATUS_OK);
        CHECK(Edit(f,"GBP 5",5U)==0);CHECK(Selection(f,1U,1U)==0);
        CHECK(UmiDocumentCoordinatorUndo(f->documents,f->id)==UMI_STATUS_OK);
        UmiUiDocumentViewSnapshot view;CHECK(umi_ui_document_view_model_find(Views(f),f->view,&view)==UMI_STATUS_OK);
        CHECK(view.cursor_offset==0U&&view.selection_length==2U);
        CHECK(Prepare(f)==0);CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"GBP ",4U)==UMI_STATUS_OK);
        CHECK(IsText(f,"GBP 5")==0);return 0;
    }
#ifdef UMICOM_EDIT_ALLOCATION_TEST
    if(strcmp(name,"allocation")==0) {
        CHECK(Selection(f,14U,5U)==0);
        unsigned prepareFailures=0U, applyFailures=0U;
        for(long attempt=0; attempt<40; ++attempt) {
            failAllocation=attempt;
            UmiStatus status=UmiDocumentCoordinatorPrepareEdit(f->documents,f->id,&f->plan);
            failAllocation=-1;
            if(status==UMI_STATUS_OK) { UmiDocumentEditPlanDestroy(f->plan);f->plan=NULL;break; }
            CHECK(status==UMI_STATUS_OUT_OF_MEMORY && f->plan==NULL);
            ++prepareFailures;
            CHECK(IsText(f,ORIGINAL)==0);
        }
        CHECK(prepareFailures>=2U);
        for(long attempt=0; attempt<40; ++attempt) {
            CHECK(Prepare(f)==0);
            failAllocation=attempt;
            UmiStatus status=UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"draft",5U);
            failAllocation=-1;
            if(status==UMI_STATUS_OK) { CHECK(IsText(f,"Umicom Notes: draft\n")==0);break; }
            CHECK(status==UMI_STATUS_OUT_OF_MEMORY);
            ++applyFailures;
            CHECK(IsText(f,ORIGINAL)==0);
            CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&state)==UMI_STATUS_OK);
            CHECK(!state.can_undo && !state.can_redo && state.selection_offset==14U && state.selection_bytes==5U);
        }
        CHECK(applyFailures>=2U);CHECK(DiskUnchanged(f)==0);
        printf("Rejected %u prepare and %u apply allocation failures without changing the draft.\n",prepareFailures,applyFailures);
        return 0;
    }
#endif
    if(strcmp(name,"empty-insert")==0){CHECK(Selection(f,0U,0U)==0);CHECK(Prepare(f)==0);CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"Draft: ",7U)==UMI_STATUS_OK);CHECK(IsText(f,"Draft: " ORIGINAL)==0);return 0;}
    if(strcmp(name,"untitled")==0){
        CHECK(umi_document_coordinator_new(f->documents,"draft.c",f->view,sizeof f->view)==UMI_STATUS_OK);
        UmiDocumentWorkingCopySnapshot active;CHECK(umi_document_coordinator_active_snapshot(f->documents,&active)==UMI_STATUS_OK);f->id=active.document_id;
        CHECK(Prepare(f)==0);CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"Notes",5U)==UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(f->documents,&active)==UMI_STATUS_OK);CHECK(!active.has_path&&active.dirty);return 0;
    }
    if(strcmp(name,"stale-save")==0) {
        CHECK(Edit(f,"Umicom Notes: typed\n",20U)==0);
        CHECK(UmiDocumentCoordinatorSyncDocument(f->documents,f->id)==UMI_STATUS_OK);
    }
    CHECK(Selection(f,14U,5U)==0);CHECK(Prepare(f)==0);
    if(strcmp(name,"copy-owned")==0||strcmp(name,"readonly-copy")==0){
        if(strcmp(name,"readonly-copy")==0){CHECK(ReadOnly(f,1)==0);CHECK(Prepare(f)==0);}
        const char *selected=NULL;size_t bytes=0U;CHECK(UmiDocumentEditPlanSelection(f->plan,&selected,&bytes)==UMI_STATUS_OK);
        CHECK(bytes==5U&&memcmp(selected,"saved",bytes)==0);
        if(strcmp(name,"copy-owned")==0)CHECK(Edit(f,"A different draft",17U)==0);
        CHECK(memcmp(selected,"saved",5U)==0);CHECK(DiskUnchanged(f)==0);return 0;
    }
    if(strcmp(name,"replace-undo")==0||strcmp(name,"delete-undo")==0||strcmp(name,"other-tab")==0||strcmp(name,"same-text")==0||strcmp(name,"consumed")==0){
        char other[UMI_UI_ID_CAPACITY]={0};if(strcmp(name,"other-tab")==0)CHECK(Another(f,other)==0);
        const char *replacement=strcmp(name,"delete-undo")==0?"":strcmp(name,"same-text")==0?"saved":"draft";
        CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,replacement,strlen(replacement))==UMI_STATUS_OK);
        CHECK(IsText(f,replacement[0]=='\0'?"Umicom Notes: \n":strcmp(replacement,"saved")==0?ORIGINAL:"Umicom Notes: draft\n")==0);
        if(strcmp(name,"consumed")==0){CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"again",5U)==UMI_STATUS_INVALID_STATE);return 0;}
        if(strcmp(name,"same-text")==0){CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&state)==UMI_STATUS_OK);CHECK(!state.can_undo&&state.selection_bytes==0U);return 0;}
        CHECK(UmiDocumentCoordinatorUndo(f->documents,f->id)==UMI_STATUS_OK);CHECK(IsText(f,ORIGINAL)==0);
        CHECK(UmiDocumentCoordinatorRedo(f->documents,f->id)==UMI_STATUS_OK);
        if(other[0]!='\0')CHECK(OtherUnchanged(f,other)==0);
        CHECK(DiskUnchanged(f)==0);return 0;
    }
    if(strcmp(name,"malformed-utf8")==0||strcmp(name,"embedded-zero")==0||strcmp(name,"oversized-input")==0){
        UmiStatus status=strcmp(name,"malformed-utf8")==0?UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"\xc0\xaf",2U):
            strcmp(name,"embedded-zero")==0?UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"A\0B",3U):
            UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"",UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES+1U);
        CHECK(status==(strcmp(name,"oversized-input")==0?UMI_STATUS_CAPACITY_EXCEEDED:UMI_STATUS_INVALID_ARGUMENT));
        CHECK(IsText(f,ORIGINAL)==0);CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&state)==UMI_STATUS_OK);CHECK(!state.can_undo);return 0;
    }
    if(strcmp(name,"readonly-apply")==0){CHECK(ReadOnly(f,1)==0);CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"draft",5U)==UMI_STATUS_PERMISSION_DENIED);CHECK(UmiDocumentCoordinatorUndo(f->documents,f->id)==UMI_STATUS_PERMISSION_DENIED);CHECK(IsText(f,ORIGINAL)==0);return 0;}
    if(strcmp(name,"closed-target")==0){CHECK(UmiDocumentCoordinatorClose(f->documents,f->id,1)==UMI_STATUS_OK);CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"draft",5U)==UMI_STATUS_NOT_FOUND);return 0;}
    if(strcmp(name,"wrong-owner")==0){
        UmiDocumentCoordinator *other=NULL;CHECK(umi_document_coordinator_create(f->store,f->workbench,NULL,&other)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorApplyEdit(other,f->plan,"draft",5U)==UMI_STATUS_INVALID_ARGUMENT);umi_document_coordinator_destroy(other);return 0;
    }
    if(strcmp(name,"stale-caret")==0)CHECK(Selection(f,0U,5U)==0);
    else if(strcmp(name,"stale-selection")==0)CHECK(Selection(f,14U,0U)==0);
    else if(strcmp(name,"stale-draft")==0)CHECK(Edit(f,"A newer note",12U)==0);
    else if(strcmp(name,"stale-store")==0)CHECK(umi_document_store_replace_text(f->store,f->id,"new store text",14U)==UMI_STATUS_OK);
    else if(strcmp(name,"stale-save")==0)CHECK(umi_document_coordinator_save_active(f->documents)==UMI_STATUS_OK);
    else if(strcmp(name,"stale-save-as")==0){char path[UMI_PATH_CAPACITY];CHECK(umi_fs_join(path,sizeof path,f->root,"renamed.c")==UMI_STATUS_OK);CHECK(UmiDocumentCoordinatorSaveAs(f->documents,f->id,path)==UMI_STATUS_OK);}
    else {fprintf(stderr,"Unknown case: %s\n",name);return 1;}
    CHECK(UmiDocumentCoordinatorApplyEdit(f->documents,f->plan,"draft",5U)==UMI_STATUS_INVALID_STATE);
    CHECK(IsText(f,strcmp(name,"stale-draft")==0?"A newer note":strcmp(name,"stale-save")==0?"Umicom Notes: typed\n":ORIGINAL)==0);
    return 0;
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    Fixture f={0};int result=Start(&f,argv[1]);if(result==0)result=Run(&f,argv[1]);Stop(&f);
    if(result==0)printf("PASS %s\n",argv[1]);
    return result;
}
