/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/close_document.c
 * PURPOSE:
 *   Follow a Notes document through Cancel, Save and Close, and a rejected
 *   outdated discard decision while a second unsaved document stays open.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/close.h"
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

/* Checks remain active in Release builds. Every exit goes through cleanup. */
#define REQUIRE(condition) do { if (!(condition)) { \
    fprintf(stderr,"Line %d: %s\n",__LINE__,#condition); result=1; goto cleanup; \
} } while(0)

/* This helper changes the visible complete draft, not the file on disk. */
static UmiStatus ChangeDraft(UmiUiWorkbench *workbench,const char *viewId,const char *text)
{
    UmiUiDocumentViewSnapshot view;
    UmiUiDocumentViewModel *views=umi_ui_workbench_documents(workbench);
    UmiStatus status=umi_ui_document_view_model_find(views,viewId,&view);
    if(status!=UMI_STATUS_OK)return status;
    view.dirty=1;
    view.cursor_offset=0U;
    view.selection_length=0U;
    return UmiUiDocumentViewModelUpsertText(views,&view,text,strlen(text));
}

int main(void)
{
    UmiCommandRegistry *commands=NULL;
    UmiUiWorkbench *workbench=NULL;
    UmiDocumentStore *store=NULL;
    UmiDocumentCoordinator *documents=NULL;
    UmiDocumentClosePlan *plan=NULL;
    UmiDocumentWorkingCopySnapshot notes,header,active;
    UmiDocumentCloseSummary summary;
    char temp[UMI_PATH_CAPACITY],root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY],leaf[80];
    char notesView[UMI_UI_ID_CAPACITY],headerView[UMI_UI_ID_CAPACITY];
    char *disk=NULL;
    size_t diskBytes=0U;
    int result=0,madeRoot=0;
    const char *savedText="int CountSavedNotes(void) { return 2; }\n";
    const char *draftText="int CountSavedNotes(void) { return 3; }\n";

    /* Own a temporary workspace. Never overwrite an existing directory. */
    REQUIRE(umi_fs_temp_directory(temp,sizeof(temp))==UMI_STATUS_OK);
    REQUIRE(snprintf(leaf,sizeof(leaf),"umicom-close-example-%ld",(long)PROCESS_ID())>0);
    REQUIRE(umi_fs_join(root,sizeof(root),temp,leaf)==UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(root));
    REQUIRE(umi_fs_make_directories(root)==UMI_STATUS_OK);madeRoot=1;
    REQUIRE(umi_fs_join(path,sizeof(path),root,"notes.c")==UMI_STATUS_OK);
    REQUIRE(umi_fs_write_text(path,savedText)==UMI_STATUS_OK);

    /* Compose the existing Framework owners; the close plan creates none. */
    REQUIRE(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("umicom.notes.closing",commands,&workbench)==UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store)==UMI_STATUS_OK);
    UmiDocumentProvider provider=umi_document_local_provider();
    REQUIRE(umi_document_coordinator_create(store,workbench,&provider,&documents)==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_open(documents,path,notesView,sizeof(notesView))==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_active_snapshot(documents,&notes)==UMI_STATUS_OK);
    REQUIRE(ChangeDraft(workbench,notesView,draftText)==UMI_STATUS_OK);

    /* Ask about notes.c, then switch to an untitled header. The question must
     * still belong to notes.c, regardless of which tab is selected later. */
    REQUIRE(UmiDocumentCoordinatorPrepareClose(documents,notes.document_id,&plan)==UMI_STATUS_OK);
    REQUIRE(UmiDocumentClosePlanSummary(plan,&summary)==UMI_STATUS_OK && summary.dirty);
    REQUIRE(umi_document_coordinator_new(documents,"notes.h",headerView,sizeof(headerView))==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_active_snapshot(documents,&header)==UMI_STATUS_OK);
    REQUIRE(ChangeDraft(workbench,headerView,"int CountSavedNotes(void);\n")==UMI_STATUS_OK);
    REQUIRE(UmiDocumentCoordinatorApplyClose(documents,plan,UMI_DOCUMENT_CLOSE_CANCEL,NULL)==UMI_STATUS_CANCELLED);
    UmiDocumentClosePlanDestroy(plan);plan=NULL;
    REQUIRE(umi_document_coordinator_count(documents)==2U);
    REQUIRE(umi_fs_read_text(path,&disk,&diskBytes)==UMI_STATUS_OK);
    REQUIRE(diskBytes==strlen(savedText) && memcmp(disk,savedText,diskBytes)==0);
    free(disk);disk=NULL;
    puts("Cancel kept both drafts open and left the saved Notes file unchanged.");

    /* Make a fresh question and choose Save and Close for the original source.
     * The independent file read checks the result, not just the return code. */
    REQUIRE(UmiDocumentCoordinatorPrepareClose(documents,notes.document_id,&plan)==UMI_STATUS_OK);
    REQUIRE(UmiDocumentCoordinatorApplyClose(documents,plan,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_OK);
    UmiDocumentClosePlanDestroy(plan);plan=NULL;
    REQUIRE(umi_document_coordinator_count(documents)==1U);
    REQUIRE(umi_document_coordinator_active_snapshot(documents,&active)==UMI_STATUS_OK);
    REQUIRE(active.document_id==header.document_id && active.dirty);
    REQUIRE(umi_fs_read_text(path,&disk,&diskBytes)==UMI_STATUS_OK);
    REQUIRE(diskBytes==strlen(draftText) && memcmp(disk,draftText,diskBytes)==0);
    free(disk);disk=NULL;
    puts("Save and Close wrote notes.c; the untitled header stayed active and unsaved.");

    /* A decision made before a new edit cannot approve losing that new edit. */
    REQUIRE(UmiDocumentCoordinatorPrepareClose(documents,header.document_id,&plan)==UMI_STATUS_OK);
    REQUIRE(ChangeDraft(workbench,headerView,"/* Notes API */\nint CountSavedNotes(void);\n")==UMI_STATUS_OK);
    REQUIRE(UmiDocumentCoordinatorApplyClose(documents,plan,UMI_DOCUMENT_CLOSE_DISCARD,NULL)==UMI_STATUS_INVALID_STATE);
    REQUIRE(umi_document_coordinator_count(documents)==1U);
    UmiDocumentClosePlanDestroy(plan);plan=NULL;
    puts("The outdated discard was rejected; the newer header draft remains open.");

cleanup:
    free(disk);
    UmiDocumentClosePlanDestroy(plan);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    if(madeRoot && umi_fs_remove_tree(root)!=UMI_STATUS_OK)result=1;
    return result;
}
