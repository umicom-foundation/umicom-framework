/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/editor_workflow/close_documents.c
 * PURPOSE:
 *   Close an Umicom Notes source and header in stages, cancel without discarding
 *   the header, then keep that header while closing another source document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/close_session.h"
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
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"Line %d: %s\n",__LINE__,#x); result=1; goto cleanup; } } while(0)

int main(void)
{
    int result=0,madeRoot=0;
    UmiCommandRegistry *commands=NULL;
    UmiUiWorkbench *workbench=NULL;
    UmiDocumentStore *store=NULL;
    UmiDocumentCoordinator *documents=NULL;
    UmiDocumentCloseSession *session=NULL;
    char temp[UMI_PATH_CAPACITY],root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY],leaf[96];
    char sourceView[UMI_UI_ID_CAPACITY],headerView[UMI_UI_ID_CAPACITY];
    char *text=NULL;size_t bytes=0U;
    const char *savedSource="int CountSavedNotes(void) { return 2; }\n";
    const char *headerText="int CountSavedNotes(void);\n";
    REQUIRE(umi_fs_temp_directory(temp,sizeof(temp))==UMI_STATUS_OK);
    REQUIRE(snprintf(leaf,sizeof(leaf),"umicom-notes-close-documents-%ld",(long)PROCESS_ID())>0);
    REQUIRE(umi_fs_join(root,sizeof(root),temp,leaf)==UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(root));
    REQUIRE(umi_fs_make_directories(root)==UMI_STATUS_OK);madeRoot=1;
    REQUIRE(umi_fs_join(path,sizeof(path),root,"notes.c")==UMI_STATUS_OK);
    REQUIRE(umi_fs_write_text(path,"int CountSavedNotes(void) { return 1; }\n")==UMI_STATUS_OK);
    REQUIRE(umi_command_registry_create(&commands)==UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("example.notes.close",commands,&workbench)==UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store)==UMI_STATUS_OK);
    UmiDocumentProvider provider=umi_document_local_provider();
    REQUIRE(umi_document_coordinator_create(store,workbench,&provider,&documents)==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_open(documents,path,sourceView,sizeof(sourceView))==UMI_STATUS_OK);
    UmiUiDocumentViewSnapshot view;
    REQUIRE(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),sourceView,&view)==UMI_STATUS_OK);
    view.dirty=1;
    REQUIRE(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench),&view,savedSource,strlen(savedSource))==UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_new(documents,"notes.h",headerView,sizeof(headerView))==UMI_STATUS_OK);
    REQUIRE(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench),headerView,&view)==UMI_STATUS_OK);
    view.dirty=1;
    REQUIRE(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench),&view,headerText,strlen(headerText))==UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot header;
    REQUIRE(umi_document_coordinator_active_snapshot(documents,&header)==UMI_STATUS_OK);

    /* The first question belongs to notes.c even though notes.h is active. */
    REQUIRE(UmiDocumentCloseSessionCreate(documents,UMI_DOCUMENT_CLOSE_ALL,0U,&session)==UMI_STATUS_OK);
    REQUIRE(UmiDocumentCloseSessionStep(session)==UMI_STATUS_OK);
    UmiDocumentCloseProgress progress;
    REQUIRE(UmiDocumentCloseSessionProgress(session,&progress)==UMI_STATUS_OK);
    REQUIRE(progress.phase==UMI_DOCUMENT_CLOSE_DECISION);
    REQUIRE(strcmp(progress.current.display_name,"notes.c")==0);
    REQUIRE(UmiDocumentCloseSessionRespond(session,UMI_DOCUMENT_CLOSE_SAVE,NULL)==UMI_STATUS_OK);
    REQUIRE(umi_fs_read_text(path,&text,&bytes)==UMI_STATUS_OK);
    REQUIRE(bytes==strlen(savedSource) && memcmp(text,savedSource,bytes)==0);
    free(text);text=NULL;

    /* Stop at the header. The source remains saved/closed; no header is lost. */
    REQUIRE(UmiDocumentCloseSessionStep(session)==UMI_STATUS_OK);
    REQUIRE(UmiDocumentCloseSessionRespond(session,UMI_DOCUMENT_CLOSE_CANCEL,NULL)==UMI_STATUS_CANCELLED);
    REQUIRE(UmiDocumentCloseSessionProgress(session,&progress)==UMI_STATUS_OK);
    REQUIRE(progress.closed==1U && progress.remaining==1U && progress.phase==UMI_DOCUMENT_CLOSE_CANCELLED);
    char message[1024];
    REQUIRE(UmiDocumentCloseProgressFormat(&progress,message,sizeof(message))==UMI_STATUS_OK);
    puts(message);
    REQUIRE(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(workbench),headerView,&text,&bytes)==UMI_STATUS_OK);
    REQUIRE(bytes==strlen(headerText) && memcmp(text,headerText,bytes)==0);
    UmiUiDocumentViewModelFreeText(text);text=NULL;
    UmiDocumentCloseSessionDestroy(session);session=NULL;

    /* A fresh Close Others remembers the header's ID, not later tab changes. */
    REQUIRE(umi_document_coordinator_open(documents,path,sourceView,sizeof(sourceView))==UMI_STATUS_OK);
    REQUIRE(UmiDocumentCloseSessionCreate(documents,UMI_DOCUMENT_CLOSE_OTHERS,header.document_id,&session)==UMI_STATUS_OK);
    REQUIRE(UmiDocumentCloseSessionStep(session)==UMI_STATUS_OK);
    REQUIRE(UmiDocumentCloseSessionProgress(session,&progress)==UMI_STATUS_OK);
    REQUIRE(progress.phase==UMI_DOCUMENT_CLOSE_COMPLETE && progress.closed==1U);
    REQUIRE(umi_document_coordinator_count(documents)==1U);
    REQUIRE(umi_document_coordinator_active_snapshot(documents,&header)==UMI_STATUS_OK && header.dirty);
    puts("The source was saved and closed. The unsaved Notes header is still open.");
cleanup:
    free(text);
    UmiDocumentCloseSessionDestroy(session);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    if(madeRoot)(void)umi_fs_remove_tree(root);
    return result;
}
