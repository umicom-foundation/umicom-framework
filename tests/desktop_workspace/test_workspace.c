/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native tests use the canonical Data Server. SQL appears only in explicit
 * failure-injection fixtures; application code never bypasses the repository.
 *---------------------------------------------------------------------------*/
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include "../../src/desktop_workspace/internal.h"
#include <limits.h>
#include <errno.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
typedef struct Fixture {
    UmiDataServer *server;
    UmiDesktopWorkspace *workspace;
    UmiDesktopWorkspaceSnapshot *a,*b;
} Fixture;
static int Init(Fixture *f)
{
    memset(f,0,sizeof *f); f->a=calloc(1,sizeof *f->a); f->b=calloc(1,sizeof *f->b);
    CHECK(f->a&&f->b); OK(umi_data_server_create_memory(&f->server));
    OK(UmiDesktopWorkspaceOpenServer(f->server,&f->workspace));
    OK(UmiDesktopWorkspaceRead(f->workspace,f->a)); return 0;
}
static void Dispose(Fixture *f)
{ UmiDesktopWorkspaceDestroy(f->workspace); umi_data_server_destroy(f->server); free(f->a);free(f->b); }
static int Save(Fixture *f,const char *text)
{
    OK(UmiDesktopWorkspaceRead(f->workspace,f->a));
    OK(UmiDesktopWorkspacePutNote(f->a,"note","Workshop",text));
    OK(UmiDesktopWorkspaceCommit(f->workspace,f->a->revision,f->a)); return 0;
}
static int Scratch(char path[1024])
{
#ifdef _WIN32
    wchar_t temp[512]; DWORD n=GetTempPathW(512,temp); CHECK(n&&n<512);
    char base[850],suffix[80];
    CHECK(WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,temp,-1,base,sizeof base,NULL,NULL)>0);
    int count=snprintf(suffix,sizeof suffix,"dw-%lu-%llu",(unsigned long)GetCurrentProcessId(),(unsigned long long)GetTickCount64());
    CHECK(count>0&&(size_t)count<sizeof suffix&&strlen(base)+strlen(suffix)<1024);
    strcpy(path,base);strcat(path,suffix);
    wchar_t wide[1024];
    CHECK(MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,path,-1,wide,1024)>0);
    CHECK(CreateDirectoryW(wide,NULL)!=0);
#else
    strcpy(path,"/tmp/umicom-desktop-workspace-XXXXXX");CHECK(mkdtemp(path)!=NULL);
#endif
    return 0;
}
static int Model(const char *name)
{
    Fixture f; if(Init(&f))return 1;
    CHECK(f.a->revision==1&&f.a->fontPoints==12&&f.a->noteCount==0);
    if(!strcmp(name,"defaults")) { OK(UmiDesktopWorkspaceValidate(f.a)); }
    else if(!strcmp(name,"put")) {
        OK(UmiDesktopWorkspacePutNote(f.a,"notes","Workshop plan","Prepare the notes."));
        CHECK(f.a->noteCount==1&&!strcmp(f.a->selectedNote,"notes"));
        OK(UmiDesktopWorkspacePutNote(f.a,"notes","Revised plan","Prepare tests."));
        CHECK(f.a->noteCount==1&&!strcmp(f.a->notes[0].body,"Prepare tests."));
    } else if(!strcmp(name,"remove")) {
        OK(UmiDesktopWorkspacePutNote(f.a,"a","A","a"));OK(UmiDesktopWorkspacePutNote(f.a,"b","B","b"));
        OK(UmiDesktopWorkspaceRemoveNote(f.a,"b"));CHECK(f.a->noteCount==1&&!strcmp(f.a->selectedNote,"a"));
        CHECK(UmiDesktopWorkspaceRemoveNote(f.a,"missing")==UMI_STATUS_NOT_FOUND);
        OK(UmiDesktopWorkspaceRemoveNote(f.a,"a"));CHECK(!f.a->noteCount&&!f.a->selectedNote[0]);
    } else if(!strcmp(name,"utf8")) {
        OK(UmiDesktopWorkspacePutNote(f.a,"notes","Café – ملاحظات","Line one\n日本語\t😀"));
        OK(UmiDesktopWorkspaceCommit(f.workspace,f.a->revision,f.a));
        OK(UmiDesktopWorkspaceRead(f.workspace,f.b));CHECK(!strcmp(f.a->notes[0].body,f.b->notes[0].body));
    } else if(!strcmp(name,"invalid_utf8")) {
        const char *bad[]={"\xc0\xaf","\xed\xa0\x80","\xf4\x90\x80\x80","\x80","\xe2\x82"};
        for(size_t i=0;i<sizeof bad/sizeof bad[0];++i)
            CHECK(UmiDesktopWorkspacePutNote(f.a,"n","Title",bad[i])==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!f.a->noteCount);
    } else if(!strcmp(name,"controls")) {
        CHECK(UmiDesktopWorkspacePutNote(f.a,"n","bad\nlabel","text")==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDesktopWorkspacePutNote(f.a,"n","label","bad\x1b[31m")==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDesktopWorkspacePutNote(f.a,"n","label","bad\x7f")==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(name,"invalid_ids")) {
        const char *bad[]={"","..",".","../note","file:note","a b","a\\b","日本語"};
        for(size_t i=0;i<sizeof bad/sizeof bad[0];++i)
            CHECK(UmiDesktopWorkspacePutNote(f.a,bad[i],"Title","body")==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(name,"limits")) {
        for(unsigned i=0;i<UMI_DESKTOP_WORKSPACE_NOTES;++i) {
            char id[40];(void)snprintf(id,sizeof id,"note-%u",i);OK(UmiDesktopWorkspacePutNote(f.a,id,"Title","Body"));
        }
        CHECK(UmiDesktopWorkspacePutNote(f.a,"extra","Title","Body")==UMI_STATUS_CAPACITY_EXCEEDED);
        OK(UmiDesktopWorkspacePutNote(f.a,"note-0","Changed","Changed"));
        char body[UMI_DESKTOP_WORKSPACE_BODY+1];memset(body,'x',sizeof body);body[sizeof body-1U]=0;
        CHECK(UmiDesktopWorkspacePutNote(f.a,"note-0","Title",body)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(name,"settings")) {
        f.a->theme=UMI_DESKTOP_WORKSPACE_DARK;f.a->fontPoints=28;f.a->sidebarVisible=0;
        OK(UmiDesktopWorkspaceCommit(f.workspace,f.a->revision,f.a));OK(UmiDesktopWorkspaceRead(f.workspace,f.b));
        CHECK(f.b->theme==UMI_DESKTOP_WORKSPACE_DARK&&f.b->fontPoints==28&&!f.b->sidebarVisible);
    } else if(!strcmp(name,"invalid_settings")) {
        f.a->fontPoints=9;CHECK(UmiDesktopWorkspaceValidate(f.a)==UMI_STATUS_INVALID_ARGUMENT);
        f.a->fontPoints=29;CHECK(UmiDesktopWorkspaceValidate(f.a)==UMI_STATUS_INVALID_ARGUMENT);
        f.a->fontPoints=12;f.a->theme=(UmiDesktopWorkspaceTheme)9;CHECK(UmiDesktopWorkspaceValidate(f.a)==UMI_STATUS_INVALID_ARGUMENT);
        f.a->theme=UMI_DESKTOP_WORKSPACE_SYSTEM;f.a->sidebarVisible=2;CHECK(UmiDesktopWorkspaceValidate(f.a)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(name,"invalid_selection")) {
        strcpy(f.a->selectedNote,"missing");CHECK(UmiDesktopWorkspaceValidate(f.a)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(name,"duplicate_ids")) {
        OK(UmiDesktopWorkspacePutNote(f.a,"a","A","a"));f.a->notes[1]=f.a->notes[0];f.a->noteCount=2;
        CHECK(UmiDesktopWorkspaceValidate(f.a)==UMI_STATUS_INVALID_ARGUMENT);
    } else if(!strcmp(name,"alias_edit")) {
        OK(UmiDesktopWorkspacePutNote(f.a,"a","A","content"));
        OK(UmiDesktopWorkspacePutNote(f.a,f.a->notes[0].id,f.a->notes[0].title,f.a->notes[0].body));
        CHECK(!strcmp(f.a->notes[0].body,"content"));
    } else { Dispose(&f);return 1; }
    Dispose(&f);return 0;
}
static int Storage(const char *name)
{
    Fixture f;if(Init(&f))return 1;
    if(!strcmp(name,"draft_isolation")) {
        OK(UmiDesktopWorkspacePutNote(f.a,"n","Draft","not saved"));
        OK(UmiDesktopWorkspaceRead(f.workspace,f.b));CHECK(!f.b->noteCount&&f.b->revision==1);
    } else if(!strcmp(name,"save")) {CHECK(!Save(&f,"Saved"));OK(UmiDesktopWorkspaceRead(f.workspace,f.b));CHECK(f.b->revision==2&&f.b->noteCount==1);}
    else if(!strcmp(name,"stale")) {
        *f.b=*f.a;CHECK(!Save(&f,"Newer"));
        CHECK(UmiDesktopWorkspaceCommit(f.workspace,f.b->revision,f.b)==UMI_STATUS_INVALID_STATE);
    } else if(!strcmp(name,"revision_mismatch")) {
        f.a->revision=99;CHECK(UmiDesktopWorkspaceCommit(f.workspace,1,f.a)==UMI_STATUS_INVALID_STATE);
    } else if(!strcmp(name,"restore")) {
        CHECK(!Save(&f,"First"));CHECK(!Save(&f,"Second"));
        OK(UmiDesktopWorkspaceRestore(f.workspace,3,2));OK(UmiDesktopWorkspaceRead(f.workspace,f.a));
        CHECK(f.a->revision==4&&!strcmp(f.a->notes[0].body,"First"));
        OK(UmiDesktopWorkspaceReadCheckpoint(f.workspace,3,f.b));CHECK(!strcmp(f.b->notes[0].body,"Second"));
    } else if(!strcmp(name,"history")) {
        for(unsigned i=0;i<20;++i){char text[20];(void)snprintf(text,sizeof text,"edit-%u",i);CHECK(!Save(&f,text));}
        CHECK(UmiDesktopWorkspaceOldestRevision(f.workspace)==14);
        CHECK(UmiDesktopWorkspaceReadCheckpoint(f.workspace,13,f.b)==UMI_STATUS_NOT_FOUND);
        for(uint64_t i=14;i<=21;++i)OK(UmiDesktopWorkspaceReadCheckpoint(f.workspace,i,f.b));
        CHECK(umi_data_server_count(f.server)==17); /* eight one-chunk snapshots + head */
    } else if(!strcmp(name,"max_snapshot")) {
        char body[4096];memset(body,'a',sizeof body-1U);body[sizeof body-1U]=0;
        for(unsigned i=0;i<16;++i){char id[40];(void)snprintf(id,sizeof id,"note-%u",i);OK(UmiDesktopWorkspacePutNote(f.a,id,"Title",body));}
        for(unsigned j=0;j<9;++j){OK(UmiDesktopWorkspaceCommit(f.workspace,f.a->revision,f.a));OK(UmiDesktopWorkspaceRead(f.workspace,f.a));}
        OK(UmiDesktopWorkspaceReadCheckpoint(f.workspace,3,f.b));CHECK(f.b->noteCount==16&&strlen(f.b->notes[15].body)==4095);
        CHECK(umi_data_server_count(f.server)<600);
    } else if(!strcmp(name,"clean_close")) {
        OK(UmiDesktopWorkspaceCloseClean(f.workspace));OK(UmiDesktopWorkspaceCloseClean(f.workspace));
        CHECK(UmiDesktopWorkspaceCommit(f.workspace,1,f.a)==UMI_STATUS_INVALID_STATE);
        UmiDesktopWorkspaceDestroy(f.workspace);f.workspace=NULL;
        OK(UmiDesktopWorkspaceOpenServer(f.server,&f.workspace));CHECK(!UmiDesktopWorkspacePreviousSessionUnfinished(f.workspace));
    } else if(!strcmp(name,"unfinished")) {
        CHECK(!Save(&f,"Last committed"));UmiDesktopWorkspaceDestroy(f.workspace);f.workspace=NULL;
        OK(UmiDesktopWorkspaceOpenServer(f.server,&f.workspace));CHECK(UmiDesktopWorkspacePreviousSessionUnfinished(f.workspace));
        OK(UmiDesktopWorkspaceRead(f.workspace,f.b));CHECK(!strcmp(f.b->notes[0].body,"Last committed"));
    } else if(!strcmp(name,"external_head")) {
        OK(umi_data_server_set(f.server,DW_HEAD,"not the observed head"));
        CHECK(UmiDesktopWorkspaceCommit(f.workspace,1,f.a)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiDesktopWorkspaceCloseClean(f.workspace)==UMI_STATUS_INVALID_STATE);
    } else if(!strcmp(name,"corrupt_chunk")) {
        OK(umi_data_server_set(f.server,"desktop.workspace.g.00000000000000000001.000","00"));
        CHECK(UmiDesktopWorkspaceReadCheckpoint(f.workspace,1,f.b)==UMI_STATUS_PARSE_ERROR);
    } else if(!strcmp(name,"corrupt_digest")) {
        char text[2050];OK(umi_data_server_get(f.server,"desktop.workspace.g.00000000000000000001.000",text,sizeof text));
        text[0]=text[0]=='0'?'1':'0';OK(umi_data_server_set(f.server,"desktop.workspace.g.00000000000000000001.000",text));
        CHECK(UmiDesktopWorkspaceReadCheckpoint(f.workspace,1,f.b)==UMI_STATUS_PARSE_ERROR);
    } else if(!strcmp(name,"missing_chunk")) {
        OK(umi_data_server_delete(f.server,"desktop.workspace.g.00000000000000000001.000"));
        CHECK(UmiDesktopWorkspaceReadCheckpoint(f.workspace,1,f.b)==UMI_STATUS_NOT_FOUND);
    } else if(!strcmp(name,"capacity_rollback")) {
        for(unsigned i=0;i<2045;++i){char key[80];(void)snprintf(key,sizeof key,"fixture.unrelated.%u",i);OK(umi_data_server_set(f.server,key,"retained"));}
        OK(UmiDesktopWorkspacePutNote(f.a,"n","Title","Body"));
        CHECK(UmiDesktopWorkspaceCommit(f.workspace,1,f.a)==UMI_STATUS_CAPACITY_EXCEEDED);
        OK(UmiDesktopWorkspaceRead(f.workspace,f.b));CHECK(f.b->revision==1&&!f.b->noteCount);
        CHECK(!umi_data_server_in_transaction(f.server));CHECK(umi_data_server_count(f.server)==2048);
    } else if(!strcmp(name,"active_transaction")) {
        OK(umi_data_server_begin(f.server));CHECK(UmiDesktopWorkspaceCommit(f.workspace,1,f.a)==UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(f.server));OK(umi_data_server_rollback(f.server));
    } else {Dispose(&f);return 1;}
    Dispose(&f);return 0;
}
static int Codec(const char *name)
{
    Fixture f;if(Init(&f))return 1;
    OK(UmiDesktopWorkspacePutNote(f.a,"notes","Café","日本語 😀\nsecond line"));
    unsigned char *data=NULL;size_t length=0;OK(DwEncode(f.a,&data,&length));
    if(!strcmp(name,"roundtrip")){OK(DwDecode(data,length,f.b));CHECK(!memcmp(f.a,f.b,sizeof *f.a));}
    else if(!strcmp(name,"truncation")){for(size_t i=0;i<length;++i)CHECK(DwDecode(data,i,f.b)!=UMI_STATUS_OK);}
    else if(!strcmp(name,"trailing")){unsigned char *longer=calloc(1,length+1U);CHECK(longer);memcpy(longer,data,length);CHECK(DwDecode(longer,length+1U,f.b)==UMI_STATUS_PARSE_ERROR);free(longer);}
    else if(!strcmp(name,"host_layout")) {
        CHECK(!memcmp(data,"UDW1",4)&&data[4]==1&&data[12]==0&&data[13]==12&&data[14]==1&&data[15]==1);
        CHECK(data[16]==5&&data[17]==0&&!memcmp(data+18,"notes",5));
    } else if(!strcmp(name,"fuzz")) {
        uint32_t state=123456789;
        for(unsigned i=0;i<10000;++i){state=state*1664525U+1013904223U;size_t at=state%length;unsigned char old=data[at];data[at]^=(unsigned char)(state>>24);UmiStatus s=DwDecode(data,length,f.b);CHECK(s==UMI_STATUS_OK||s==UMI_STATUS_PARSE_ERROR);if(s==UMI_STATUS_OK)OK(UmiDesktopWorkspaceValidate(f.b));data[at]=old;}
    } else {free(data);Dispose(&f);return 1;}
    free(data);Dispose(&f);return 0;
}
static int BadRecord(const char *name)
{
    Fixture f;if(Init(&f))return 1;
    UmiDesktopWorkspaceDestroy(f.workspace);f.workspace=NULL;
    if(!strcmp(name,"head_overflow"))OK(umi_data_server_set(f.server,DW_HEAD,"UDWH1\nrevision=18446744073709551616\noldest=1\nclean=1\n"));
    else if(!strcmp(name,"head_sign"))OK(umi_data_server_set(f.server,DW_HEAD,"UDWH1\nrevision=-1\noldest=1\nclean=1\n"));
    else if(!strcmp(name,"head_schema"))OK(umi_data_server_set(f.server,DW_HEAD,"UDWH2\nrevision=1\noldest=1\nclean=1\n"));
    else if(!strcmp(name,"head_zero"))OK(umi_data_server_set(f.server,DW_HEAD,"UDWH1\nrevision=01\noldest=1\nclean=1\n"));
    else if(!strcmp(name,"head_tail"))OK(umi_data_server_set(f.server,DW_HEAD,"UDWH1\nrevision=1\noldest=1\nclean=1\nextra=1\n"));
    else if(!strcmp(name,"manifest_overflow"))OK(umi_data_server_set(f.server,"desktop.workspace.g.00000000000000000001.meta","UDWG1\nbytes=999999999999999999999999999999999999\nchunks=1\nsha256=abc\n"));
    else if(!strcmp(name,"unrelated_store")){OK(umi_data_server_delete(f.server,DW_HEAD));}
    else {Dispose(&f);return 1;}
    size_t before=umi_data_server_count(f.server);
    CHECK(UmiDesktopWorkspaceOpenServer(f.server,&f.workspace)!=UMI_STATUS_OK);CHECK(f.workspace==NULL);
    CHECK(umi_data_server_count(f.server)==before);Dispose(&f);return 0;
}
static int Sqlite(const char *name)
{
    char root[1024],path[1100];CHECK(!Scratch(root));int n=snprintf(path,sizeof path,"%s/direct.sqlite",root);CHECK(n>0&&(size_t)n<sizeof path);
    Fixture f={0};f.a=calloc(1,sizeof *f.a);f.b=calloc(1,sizeof *f.b);CHECK(f.a&&f.b);
    UmiStatus status=umi_data_server_create_sqlite(path,&f.server);
    if(status==UMI_STATUS_UNAVAILABLE){free(f.a);free(f.b);puts("NOT RUN: SQLite disabled or unavailable.");return 77;}
    OK(status);OK(UmiDesktopWorkspaceOpenServer(f.server,&f.workspace));
    if(!strcmp(name,"restart")) {
        CHECK(!Save(&f,"Persistent note"));OK(UmiDesktopWorkspaceCloseClean(f.workspace));
        UmiDesktopWorkspaceDestroy(f.workspace);umi_data_server_destroy(f.server);f.workspace=NULL;f.server=NULL;
        OK(umi_data_server_create_sqlite(path,&f.server));OK(UmiDesktopWorkspaceOpenServer(f.server,&f.workspace));
        OK(UmiDesktopWorkspaceRead(f.workspace,f.a));CHECK(f.a->revision==2&&!strcmp(f.a->notes[0].body,"Persistent note"));
        CHECK(!UmiDesktopWorkspacePreviousSessionUnfinished(f.workspace));
    } else if(!strcmp(name,"write_failure")) {
        OK(umi_data_server_execute(f.server,"CREATE TRIGGER fail_head BEFORE UPDATE ON umicom_kv WHEN NEW.key='desktop.workspace.head' BEGIN SELECT RAISE(FAIL,'injected head failure'); END;"));
        OK(UmiDesktopWorkspaceRead(f.workspace,f.a));OK(UmiDesktopWorkspacePutNote(f.a,"n","Title","not committed"));
        CHECK(UmiDesktopWorkspaceCommit(f.workspace,1,f.a)==UMI_STATUS_IO_ERROR);
        CHECK(umi_data_server_count(f.server)==3&&!umi_data_server_in_transaction(f.server));
        OK(UmiDesktopWorkspaceRead(f.workspace,f.b));CHECK(f.b->revision==1);
        OK(umi_data_server_execute(f.server,"DROP TRIGGER fail_head;"));OK(UmiDesktopWorkspaceCommit(f.workspace,1,f.a));
    } else if(!strcmp(name,"rollback_failure")) {
        OK(umi_data_server_execute(f.server,"CREATE TRIGGER rollback_head BEFORE UPDATE ON umicom_kv WHEN NEW.key='desktop.workspace.head' BEGIN SELECT RAISE(ROLLBACK,'injected transaction loss'); END;"));
        OK(UmiDesktopWorkspaceRead(f.workspace,f.a));
        CHECK(UmiDesktopWorkspaceCommit(f.workspace,1,f.a)==UMI_STATUS_IO_ERROR);
        CHECK(f.workspace->poisoned);CHECK(UmiDesktopWorkspaceCloseClean(f.workspace)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiDesktopWorkspaceRead(f.workspace,f.b)==UMI_STATUS_INVALID_STATE);
    } else if(!strcmp(name,"retire_atomicity")) {
        for(unsigned i=0;i<7;++i)CHECK(!Save(&f,"history"));
        size_t count=umi_data_server_count(f.server);
        OK(umi_data_server_execute(f.server,"CREATE TRIGGER fail_retire BEFORE DELETE ON umicom_kv WHEN OLD.key='desktop.workspace.g.00000000000000000001.meta' BEGIN SELECT RAISE(FAIL,'retain original'); END;"));
        OK(UmiDesktopWorkspaceRead(f.workspace,f.a));CHECK(UmiDesktopWorkspaceCommit(f.workspace,8,f.a)==UMI_STATUS_IO_ERROR);
        CHECK(umi_data_server_count(f.server)==count);CHECK(UmiDesktopWorkspaceOldestRevision(f.workspace)==1);
        OK(UmiDesktopWorkspaceReadCheckpoint(f.workspace,1,f.b));
    } else {Dispose(&f);return 1;}
    Dispose(&f);return 0;
}
static int Local(const char *name)
{
    char parent[1024],root[1100];CHECK(!Scratch(parent));int n=snprintf(root,sizeof root,"%s/workspace",parent);CHECK(n>0&&(size_t)n<sizeof root);
    UmiDesktopWorkspace *w=NULL,*other=NULL;
    UmiStatus status=UmiDesktopWorkspaceOpenDirectory(root,&w);
    if(status==UMI_STATUS_UNAVAILABLE){puts("NOT RUN: SQLite unavailable.");return 77;}
    OK(status);
    if(!strcmp(name,"lock")){CHECK(UmiDesktopWorkspaceOpenDirectory(root,&other)==UMI_STATUS_BUSY);CHECK(!other);}
    else if(!strcmp(name,"reopen")){OK(UmiDesktopWorkspaceCloseClean(w));UmiDesktopWorkspaceDestroy(w);w=NULL;OK(UmiDesktopWorkspaceOpenDirectory(root,&w));CHECK(!UmiDesktopWorkspacePreviousSessionUnfinished(w));}
#ifndef _WIN32
    else if(!strcmp(name,"symlink")) {
        char linkPath[1100];n=snprintf(linkPath,sizeof linkPath,"%s/link",parent);CHECK(n>0&&(size_t)n<sizeof linkPath);CHECK(symlink(root,linkPath)==0);
        CHECK(UmiDesktopWorkspaceOpenDirectory(linkPath,&other)!=UMI_STATUS_OK&&!other);
    } else if(!strcmp(name,"private_directory")) {
        UmiDesktopWorkspaceDestroy(w);w=NULL;CHECK(chmod(root,0755)==0);
        CHECK(UmiDesktopWorkspaceOpenDirectory(root,&w)==UMI_STATUS_PERMISSION_DENIED&&!w);
    } else if(!strcmp(name,"database_link")) {
        UmiDesktopWorkspaceDestroy(w);w=NULL;
        char db[1200],alias[1200];(void)snprintf(db,sizeof db,"%s/workspace.sqlite",root);(void)snprintf(alias,sizeof alias,"%s/alias",parent);
        CHECK(link(db,alias)==0);CHECK(UmiDesktopWorkspaceOpenDirectory(root,&w)==UMI_STATUS_INVALID_ARGUMENT&&!w);
    } else if(!strcmp(name,"child_exit")) {
        OK(UmiDesktopWorkspaceCloseClean(w));UmiDesktopWorkspaceDestroy(w);w=NULL;
        pid_t child=fork();CHECK(child>=0);
        if(child==0){UmiDesktopWorkspace*c=NULL;if(UmiDesktopWorkspaceOpenDirectory(root,&c)!=UMI_STATUS_OK)_exit(2);_exit(0);}
        int result=0;CHECK(waitpid(child,&result,0)==child&&WIFEXITED(result)&&WEXITSTATUS(result)==0);
        OK(UmiDesktopWorkspaceOpenDirectory(root,&w));CHECK(UmiDesktopWorkspacePreviousSessionUnfinished(w));
    } else if(!strcmp(name,"child_lock")) {
        pid_t child=fork();CHECK(child>=0);
        if(child==0){UmiDesktopWorkspace*c=NULL;UmiStatus s=UmiDesktopWorkspaceOpenDirectory(root,&c);_exit(s==UMI_STATUS_BUSY?0:2);}
        int result=0;CHECK(waitpid(child,&result,0)==child&&WIFEXITED(result)&&WEXITSTATUS(result)==0);
    }
#endif
    else {UmiDesktopWorkspaceDestroy(w);return 1;}
    UmiDesktopWorkspaceDestroy(w);return 0;
}
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(!strcmp(argv[1],"model"))return Model(argv[2]);
    if(!strcmp(argv[1],"storage"))return Storage(argv[2]);
    if(!strcmp(argv[1],"codec"))return Codec(argv[2]);
    if(!strcmp(argv[1],"bad"))return BadRecord(argv[2]);
    if(!strcmp(argv[1],"sqlite"))return Sqlite(argv[2]);
    if(!strcmp(argv[1],"local"))return Local(argv[2]);
    return 2;
}
