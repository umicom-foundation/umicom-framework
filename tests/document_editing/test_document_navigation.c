/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_editing/test_document_navigation.c
 * PURPOSE: Verify counted location input and real document navigation outcomes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/navigation.h"
#include "umicom/document/edit.h"
#include "umicom/document/commands.h"
#include "umicom/document/local_provider.h"
#include "umicom/application_shell/profiles/navigate_menu.h"
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
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
#define TEXT "Umicom Notes\nA\xc2\xa3" "B\nlast\n"
typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiDocumentId id;
    char view[UMI_UI_ID_CAPACITY];
    char root[UMI_PATH_CAPACITY],path[UMI_PATH_CAPACITY];
    int madeRoot;
} Fixture;
static UmiUiDocumentViewModel *Views(Fixture *f) { return umi_ui_workbench_documents(f->workbench); }
static int Setup(Fixture *f,const char *name)
{
    char tmp[UMI_PATH_CAPACITY],leaf[128];
    CHECK(umi_fs_temp_directory(tmp,sizeof tmp)==UMI_STATUS_OK);
    CHECK(snprintf(leaf,sizeof leaf,"umicom-navigation-%ld-%s",(long)GETPID(),name)>0);
    CHECK(umi_fs_join(f->root,sizeof f->root,tmp,leaf)==UMI_STATUS_OK);
    CHECK(!umi_fs_exists(f->root));
    CHECK(umi_fs_make_directories(f->root)==UMI_STATUS_OK);f->madeRoot=1;
    CHECK(umi_fs_join(f->path,sizeof f->path,f->root,"notes.c")==UMI_STATUS_OK);
    CHECK(umi_fs_write_text(f->path,TEXT)==UMI_STATUS_OK);
    CHECK(umi_command_registry_create(&f->commands)==UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("navigation.test",f->commands,&f->workbench)==UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store)==UMI_STATUS_OK);
    UmiDocumentProvider provider=umi_document_local_provider();
    CHECK(umi_document_coordinator_create(f->store,f->workbench,&provider,&f->documents)==UMI_STATUS_OK);
    CHECK(umi_document_coordinator_open(f->documents,f->path,f->view,sizeof f->view)==UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f->documents,&active)==UMI_STATUS_OK);
    f->id=active.document_id;
    return 0;
}
static void Cleanup(Fixture *f)
{
    umi_document_coordinator_destroy(f->documents);
    umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench);
    umi_command_registry_destroy(f->commands);
    if(f->madeRoot)(void)umi_fs_remove_tree(f->root);
}
static int DiskIntact(Fixture *f)
{
    char *text=NULL;size_t bytes=0;
    CHECK(umi_fs_read_text(f->path,&text,&bytes)==UMI_STATUS_OK);
    int equal=bytes==strlen(TEXT)&&memcmp(text,TEXT,bytes)==0;
    free(text);CHECK(equal);return 0;
}
static int Reject(const char *input,size_t bytes,UmiStatus expected)
{
    UmiDocumentLocation location={21,34};
    CHECK(UmiDocumentLocationParse(input,bytes,&location)==expected);
    CHECK(location.line==21 && location.column==34);
    return 0;
}
static int Parser(const char *name)
{
    UmiDocumentLocation location={0};
    if(strcmp(name,"parse-line")==0) {
        CHECK(UmiDocumentLocationParse("12",2,&location)==UMI_STATUS_OK);
        CHECK(location.line==12&&location.column==1);
    } else if(strcmp(name,"parse-column")==0) {
        CHECK(UmiDocumentLocationParse("12:5",4,&location)==UMI_STATUS_OK);
        CHECK(location.line==12&&location.column==5);
    } else if(strcmp(name,"parse-spaces")==0) {
        const char text[]=" \t012:005\t ";
        CHECK(UmiDocumentLocationParse(text,sizeof text-1,&location)==UMI_STATUS_OK);
        CHECK(location.line==12&&location.column==5);
    } else if(strcmp(name,"parse-signs")==0) {
        const char *bad[]={"-1","+1","1:-1","1:+1"};
        for(size_t i=0;i<4;++i)CHECK(Reject(bad[i],strlen(bad[i]),UMI_STATUS_INVALID_ARGUMENT)==0);
    } else if(strcmp(name,"parse-zero")==0) {
        const char *bad[]={"0","00","1:0","0:1","\t  "};
        for(size_t i=0;i<5;++i)CHECK(Reject(bad[i],strlen(bad[i]),UMI_STATUS_INVALID_ARGUMENT)==0);
    } else if(strcmp(name,"parse-separators")==0) {
        const char *bad[]={":1","1:","1:2:3","1 2","1 :2","1:\t2","1\n","1.5","0x12"};
        for(size_t i=0;i<9;++i)CHECK(Reject(bad[i],strlen(bad[i]),UMI_STATUS_INVALID_ARGUMENT)==0);
    } else if(strcmp(name,"parse-overflow")==0) {
        char text[80];int n=snprintf(text,sizeof text,"%zu0",SIZE_MAX);CHECK(n>0);
        CHECK(Reject(text,(size_t)n,UMI_STATUS_INVALID_ARGUMENT)==0);
        n=snprintf(text,sizeof text,"1:%zu0",SIZE_MAX);CHECK(n>0);
        CHECK(Reject(text,(size_t)n,UMI_STATUS_INVALID_ARGUMENT)==0);
    } else if(strcmp(name,"parse-boundaries")==0) {
        char text[97];int n=snprintf(text,sizeof text,"%zu:%zu",SIZE_MAX,SIZE_MAX);CHECK(n>0);
        CHECK(UmiDocumentLocationParse(text,(size_t)n,&location)==UMI_STATUS_OK);
        CHECK(location.line==SIZE_MAX&&location.column==SIZE_MAX);
        memset(text,' ',sizeof text);text[0]='1';
        CHECK(UmiDocumentLocationParse(text,96,&location)==UMI_STATUS_OK);
        CHECK(Reject(text,97,UMI_STATUS_CAPACITY_EXCEEDED)==0);
        CHECK(Reject("1",SIZE_MAX,UMI_STATUS_CAPACITY_EXCEEDED)==0);
    } else if(strcmp(name,"parse-counted")==0) {
        const char exact[]={'4',':','2'};
        CHECK(UmiDocumentLocationParse(exact,sizeof exact,&location)==UMI_STATUS_OK);
        CHECK(location.line==4&&location.column==2);
        CHECK(Reject("1\0:2",4,UMI_STATUS_INVALID_ARGUMENT)==0);
        CHECK(Reject("\xef\xbc\x91",3,UMI_STATUS_INVALID_ARGUMENT)==0);
    } else if(strcmp(name,"invalid-arguments")==0) {
        CHECK(Reject(NULL,1,UMI_STATUS_INVALID_ARGUMENT)==0);
        CHECK(Reject("",0,UMI_STATUS_INVALID_ARGUMENT)==0);
        CHECK(UmiDocumentLocationParse("1",1,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorNavigate(NULL,1,"1",1,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
static int Navigate(Fixture *f,const char *name)
{
    UmiUiDocumentViewSnapshot before,after;
    CHECK(umi_ui_document_view_model_find(Views(f),f->view,&before)==UMI_STATUS_OK);
    before.cursor_offset=1;before.selection_length=3;
    CHECK(umi_ui_document_view_model_upsert(Views(f),&before)==UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot savedBefore,savedAfter;
    CHECK(umi_document_coordinator_active_snapshot(f->documents,&savedBefore)==UMI_STATUS_OK);
    size_t offset=9876;
    if(strcmp(name,"navigate-location")==0) {
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,"2:4",3,&offset)==UMI_STATUS_OK);
        CHECK(offset==16);
    } else if(strcmp(name,"navigate-byte-column")==0) {
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,"2:3",3,&offset)==UMI_STATUS_OK);
        CHECK(offset==14); /* Inside the pound sign selects its first UTF-8 byte. */
    } else if(strcmp(name,"navigate-end-clamp")==0) {
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,"2:99999",7,&offset)==UMI_STATUS_OK);
        CHECK(offset==17);
    } else if(strcmp(name,"navigate-empty-final-line")==0) {
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,"4",1,&offset)==UMI_STATUS_OK);
        CHECK(offset==strlen(TEXT));
    } else if(strcmp(name,"navigate-missing-line")==0 || strcmp(name,"navigate-invalid")==0) {
        int missing=strcmp(name,"navigate-missing-line")==0;
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,missing?"99":"-1",2,&offset)==
            (missing?UMI_STATUS_NOT_FOUND:UMI_STATUS_INVALID_ARGUMENT));
        CHECK(offset==9876);
        CHECK(umi_ui_document_view_model_find(Views(f),f->view,&after)==UMI_STATUS_OK);
        CHECK(after.cursor_offset==before.cursor_offset&&after.selection_length==before.selection_length);
        return DiskIntact(f);
    } else if(strcmp(name,"navigate-readonly")==0) {
        before.read_only=1;CHECK(umi_ui_document_view_model_upsert(Views(f),&before)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,"2",1,&offset)==UMI_STATUS_OK);
        CHECK(offset==13);
    } else if(strcmp(name,"navigate-other-tab")==0) {
        char other[UMI_UI_ID_CAPACITY];
        CHECK(umi_document_coordinator_new(f->documents,"review.txt",other,sizeof other)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,"2",1,&offset)==UMI_STATUS_INVALID_STATE);
        CHECK(offset==9876);
        CHECK(umi_ui_document_view_model_find(Views(f),f->view,&after)==UMI_STATUS_OK);
        CHECK(after.cursor_offset==before.cursor_offset&&after.selection_length==before.selection_length);
        CHECK(umi_document_coordinator_active_snapshot(f->documents,&savedAfter)==UMI_STATUS_OK);
        CHECK(strcmp(savedAfter.view_id,other)==0);return DiskIntact(f);
    } else if(strcmp(name,"navigate-closed")==0) {
        CHECK(umi_document_coordinator_close_active(f->documents,1)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,"2",1,&offset)==UMI_STATUS_NOT_FOUND);
        CHECK(offset==9876);return DiskIntact(f);
    } else if(strcmp(name,"navigate-history")==0) {
        before.dirty=1;before.cursor_offset=0;before.selection_length=0;
        const char *draft="Umicom Notes\nunsaved draft\n";
        CHECK(UmiUiDocumentViewModelUpsertText(Views(f),&before,draft,strlen(draft))==UMI_STATUS_OK);
        UmiDocumentEditState editBefore,editAfter;
        CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&editBefore)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,"2:2",3,&offset)==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorGetEditState(f->documents,f->id,&editAfter)==UMI_STATUS_OK);
        CHECK(editAfter.can_undo==editBefore.can_undo && editAfter.can_redo==editBefore.can_redo);
        char *text=NULL;size_t bytes=0;
        CHECK(UmiUiDocumentViewModelCopyText(Views(f),f->view,&text,&bytes)==UMI_STATUS_OK);
        int same=bytes==strlen(draft)&&memcmp(text,draft,bytes)==0;UmiUiDocumentViewModelFreeText(text);CHECK(same);
        CHECK(umi_document_coordinator_active_snapshot(f->documents,&savedAfter)==UMI_STATUS_OK);
        CHECK(savedAfter.dirty);return DiskIntact(f);
    } else if(strcmp(name,"navigate-large-tail")==0) {
        size_t bytes=120002;char *text=malloc(bytes+1);CHECK(text!=NULL);
        memset(text,'a',bytes);text[120000]='\n';text[120001]='Z';text[bytes]='\0';
        before.dirty=1;before.cursor_offset=0;before.selection_length=0;
        UmiStatus result=UmiUiDocumentViewModelUpsertText(Views(f),&before,text,bytes);free(text);
        CHECK(result==UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorNavigate(f->documents,f->id,"2:1",3,&offset)==UMI_STATUS_OK);
        CHECK(offset==120001);return DiskIntact(f);
    } else if(strcmp(name,"registered-command")==0 || strcmp(name,"registered-invalid")==0) {
        CHECK(umi_document_commands_register(f->commands,f->documents)==UMI_STATUS_OK);
        char message[256];int invalid=strcmp(name,"registered-invalid")==0;
        UmiStatus status=umi_command_registry_execute(f->commands,UMI_DOCUMENT_COMMAND_GO_TO_LINE,
            invalid?"-1":"2:4",message,sizeof message);
        CHECK(status==(invalid?UMI_STATUS_INVALID_ARGUMENT:UMI_STATUS_OK));
        CHECK(umi_ui_document_view_model_find(Views(f),f->view,&after)==UMI_STATUS_OK);
        CHECK(after.cursor_offset==(invalid?1U:16U));
        CHECK(strstr(message,invalid?"positive":"column")!=NULL);return DiskIntact(f);
    } else return 2;
    CHECK(umi_ui_document_view_model_find(Views(f),f->view,&after)==UMI_STATUS_OK);
    CHECK(after.cursor_offset==offset&&after.selection_length==0);
    CHECK(umi_document_coordinator_active_snapshot(f->documents,&savedAfter)==UMI_STATUS_OK);
    CHECK(savedAfter.document_id==savedBefore.document_id&&savedAfter.dirty==savedBefore.dirty);
    return DiskIntact(f);
}
int main(int argc,char **argv)
{
    if(argc!=2)return 2;
    if(strncmp(argv[1],"parse-",6)==0 || strcmp(argv[1],"invalid-arguments")==0)return Parser(argv[1]);
    if(strcmp(argv[1],"menu-contribution")==0) {
        const UmiApplicationShellProfileDefinition *profile=umi_application_shell_profile_navigate_menu();
        size_t count=0;
        for(size_t i=0;i<profile->contribution_count;++i)
            if(strcmp(profile->contributions[i].command_id,UMI_DOCUMENT_COMMAND_GO_TO_LINE)==0)++count;
        CHECK(count==1);CHECK(profile->contribution_count>=10);return 0;
    }
    Fixture f={0};int result=Setup(&f,argv[1]);
    if(result==0)result=Navigate(&f,argv[1]);
    Cleanup(&f);
    return result;
}
