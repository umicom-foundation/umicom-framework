/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_source_navigation/test_open.c
 * PURPOSE: Check actual document navigation, missing sources and unsaved-draft preservation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_ui/source_navigation.h"
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
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)

int main(int argc, char **argv)
{
    CHECK(argc == 2); const char *name=argv[1];
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], leaf[120];
    OK(umi_fs_temp_directory(temp, sizeof(temp)));
    (void)snprintf(leaf, sizeof(leaf), "umicom-test-source-%ld-%s", (long)GETPID(), name);
    OK(umi_fs_join(root, sizeof(root), temp, leaf)); CHECK(!umi_fs_exists(root));
    OK(umi_fs_make_directories(root)); OK(umi_fs_join(path, sizeof(path), root, "source notes.c"));
    const char *saved="first\nA\xc2\xa3" "B\nlast\n";
    OK(umi_fs_write_text(path, saved));
    UmiCommandRegistry *commands=NULL; UmiUiWorkbench *workbench=NULL;
    UmiDocumentStore *store=NULL; UmiDocumentCoordinator *documents=NULL;
    OK(umi_command_registry_create(&commands)); OK(umi_ui_workbench_create("source.test", commands, &workbench));
    OK(umi_document_store_create(&store)); UmiDocumentProvider provider=umi_document_local_provider();
    OK(umi_document_coordinator_create(store, workbench, &provider, &documents));
    char view[UMI_UI_ID_CAPACITY]; OK(umi_document_coordinator_open(documents, path, view, sizeof(view)));
    UmiUiDocumentViewModel *views=umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot before, after; OK(umi_ui_document_view_model_find(views, view, &before));
    before.cursor_offset=1; before.selection_length=2; OK(umi_ui_document_view_model_upsert(views, &before));
    UmiTestSourceLink link={0}; strcpy(link.item_id,"selected"); strcpy(link.record_id,"result");
    link.origin=UMI_TEST_SOURCE_RESULT_DETAILS; link.location.severity=UMI_DIAGNOSTIC_ERROR;
    strcpy(link.location.path,path); strcpy(link.location.message,"assertion"); link.location.line=2; link.location.column=4;
    size_t offset=9876; UmiStatus status; int expectUnchanged=1;
    if (strcmp(name,"absolute")==0 || strcmp(name,"relative")==0 || strcmp(name,"draft")==0) {
        if (strcmp(name,"relative")==0) strcpy(link.location.path,"source notes.c");
        const char *draft="first\nunsaved text\nlast\n";
        if (strcmp(name,"draft")==0) { before.dirty=1; OK(UmiUiDocumentViewModelUpsertText(views,&before,draft,strlen(draft))); }
        expectUnchanged=0; OK(UmiTestSourceLinkOpen(documents,&link,root,&offset)); CHECK(offset==9);
        char *text=NULL; size_t bytes=0; OK(UmiUiDocumentViewModelCopyText(views,view,&text,&bytes));
        CHECK(strcmp(text,strcmp(name,"draft")==0?draft:saved)==0); UmiUiDocumentViewModelFreeText(text);
        CHECK(umi_document_coordinator_count(documents)==1);
    } else if (strcmp(name,"other-tab")==0 || strcmp(name,"new-tab-missing-line")==0) {
        const char *draft="first\nretain this other draft\n";
        before.dirty=1; OK(UmiUiDocumentViewModelUpsertText(views,&before,draft,strlen(draft)));
        OK(umi_fs_join(link.location.path,sizeof(link.location.path),root,"other.c"));
        OK(umi_fs_write_text(link.location.path,saved));
        int missingLine=strcmp(name,"new-tab-missing-line")==0;
        link.location.line=missingLine?999U:2U;
        CHECK(UmiTestSourceLinkOpen(documents,&link,root,&offset)==(missingLine?UMI_STATUS_NOT_FOUND:UMI_STATUS_OK));
        CHECK(offset==(missingLine?9876U:9U)); expectUnchanged=0;
        UmiDocumentWorkingCopySnapshot active; OK(umi_document_coordinator_active_snapshot(documents,&active));
        CHECK(strcmp(active.view_id,view)!=0 && umi_document_coordinator_count(documents)==2);
        OK(umi_ui_document_view_model_find(views,view,&after));
        CHECK(after.cursor_offset==1 && after.selection_length==2 && after.dirty);
        char *text=NULL; size_t length=0; OK(UmiUiDocumentViewModelCopyText(views,view,&text,&length));
        CHECK(strcmp(text,draft)==0); UmiUiDocumentViewModelFreeText(text);
        CHECK(remove(link.location.path)==0);
    } else if (strcmp(name,"file-uri")==0) {
        UmiTestPlatformItemSnapshot item={0};
        OK(umi_document_uri_from_path(path,item.source_uri,sizeof(item.source_uri))); item.source_line=2;
        expectUnchanged=0; OK(UmiTestItemOpenSource(documents,&item,NULL,&offset)); CHECK(offset==6);
    } else if (strcmp(name,"missing-file")==0) {
        OK(umi_fs_join(link.location.path,sizeof(link.location.path),root,"missing.c"));
        status=UmiTestSourceLinkOpen(documents,&link,root,&offset); CHECK(status!=UMI_STATUS_OK);
    } else if (strcmp(name,"missing-line")==0) {
        link.location.line=999; CHECK(UmiTestSourceLinkOpen(documents,&link,root,&offset)==UMI_STATUS_NOT_FOUND);
    } else if (strcmp(name,"explicit-base")==0) {
        strcpy(link.location.path,"source notes.c"); CHECK(UmiTestSourceLinkOpen(documents,&link,NULL,&offset)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiTestSourceLinkOpen(documents,&link,"relative-base",&offset)==UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name,"invalid-location")==0) {
        strcpy(link.location.path,"https://example.test/test.c");
        CHECK(UmiTestSourceLinkOpen(documents,&link,root,&offset)==UMI_STATUS_NOT_FOUND);
        memset(link.location.path,'x',sizeof(link.location.path));
        CHECK(UmiTestSourceLinkOpen(documents,&link,root,&offset)==UMI_STATUS_INVALID_ARGUMENT);
        link.location.path[1500]='\0'; CHECK(UmiTestSourceLinkOpen(documents,&link,root,&offset)==UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (strcmp(name,"discovered")==0) {
        expectUnchanged=0; UmiTestPlatformItemSnapshot item={0}; strcpy(item.source_uri,path); item.source_line=2;
        OK(UmiTestItemOpenSource(documents,&item,root,&offset)); CHECK(offset==6);
        item.source_line=0; OK(UmiTestItemOpenSource(documents,&item,root,&offset)); CHECK(offset==0);
    } else return 2;
    if (expectUnchanged) {
        CHECK(offset==9876);
        OK(umi_ui_document_view_model_find(views,view,&after));
        CHECK(after.cursor_offset==before.cursor_offset && after.selection_length==before.selection_length);
        UmiDocumentWorkingCopySnapshot active; OK(umi_document_coordinator_active_snapshot(documents,&active)); CHECK(strcmp(active.view_id,view)==0);
        CHECK(umi_document_coordinator_count(documents)==1);
    }
    char *disk=NULL; size_t bytes=0; OK(umi_fs_read_text(path,&disk,&bytes)); CHECK(strcmp(disk,saved)==0); free(disk);
    umi_document_coordinator_destroy(documents); umi_document_store_destroy(store);
    umi_ui_workbench_destroy(workbench); umi_command_registry_destroy(commands);
    CHECK(remove(path)==0); OK(umi_fs_remove_tree(root));
    return 0;
}
