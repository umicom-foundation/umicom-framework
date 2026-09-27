/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Regression laboratory: compile the actual Data Server implementation here
 * so SQLite failure injection can access its private connection. No substitute
 * Data Server is used. Production callers must use public contracts instead. */
#include "../../src/data/data_server.c"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); return 1; } } while (0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)

typedef struct Work {
    UmiDataServer *server;
    int action;
    UmiStatus result;
    char value[64];
} Work;
static UmiStatus Visitor(const char *key,const char *value,void *context)
{
    (void)key; (void)value; ++*(unsigned *)context; return UMI_STATUS_OK;
}
static void Perform(Work *work)
{
    size_t count=0; unsigned visits=0;
    UmiDataServerSnapshot snapshot;
    switch (work->action) {
    case 0: work->result=umi_data_server_set(work->server,"competing","value"); break;
    case 1: work->result=umi_data_server_get(work->server,"note",work->value,sizeof work->value); break;
    case 2: work->result=umi_data_server_delete(work->server,"note"); break;
    case 3: work->result=umi_data_server_begin(work->server); break;
    case 4: work->result=umi_data_server_commit(work->server); break;
    case 5: work->result=umi_data_server_rollback(work->server); break;
    case 6: work->result=UmiDataServerCountChecked(work->server,&count); break;
    case 7: work->result=umi_data_server_visit(work->server,Visitor,&visits); break;
    case 8: work->result=umi_data_server_snapshot(work->server,&snapshot); break;
    case 9: work->result=umi_data_server_execute(work->server,"DELETE FROM umicom_kv;"); break;
    default: work->result=UMI_STATUS_INVALID_ARGUMENT; break;
    }
}
#ifdef _WIN32
static DWORD WINAPI WorkerEntry(LPVOID data) { Perform(data); return 0; }
static int Join(Work *work)
{
    HANDLE thread=CreateThread(NULL,0,WorkerEntry,work,0,NULL);
    if (thread==NULL) return 1;
    DWORD status=WaitForSingleObject(thread,INFINITE); CloseHandle(thread);
    return status!=WAIT_OBJECT_0;
}
#else
static void *WorkerEntry(void *data) { Perform(data); return NULL; }
static int Join(Work *work)
{
    pthread_t thread;
    if (pthread_create(&thread,NULL,WorkerEntry,work)!=0) return 1;
    return pthread_join(thread,NULL)!=0;
}
#endif
static UmiStatus Reenter(const char *key,const char *value,void *context)
{
    (void)key; (void)value;
    Work *work=context; Perform(work); return UMI_STATUS_OK;
}
static UmiStatus StopVisitor(const char *key,const char *value,void *context)
{
    (void)key; (void)value; (void)context; return UMI_STATUS_CANCELLED;
}
#ifdef UMICOM_HAS_SQLITE
static int DenyTransaction(void *context,int action,const char *arg,const char *b,const char *c,const char *d)
{
    (void)b; (void)c; (void)d;
    return action==SQLITE_TRANSACTION && arg && strcmp(arg,(const char *)context)==0 ? SQLITE_DENY : SQLITE_OK;
}
#endif
static int Exercise(UmiDataServer *server,const char *name)
{
    char value[64]; size_t count=99;
    if (!strcmp(name,"roundtrip")) {
        OK(umi_data_server_set(server,"note","saved")); OK(umi_data_server_get(server,"note",value,sizeof value)); CHECK(!strcmp(value,"saved"));
        OK(UmiDataServerCountChecked(server,&count)); CHECK(count==1);
    } else if (!strcmp(name,"rollback")) {
        OK(umi_data_server_set(server,"note","saved")); OK(umi_data_server_begin(server)); OK(umi_data_server_set(server,"note","draft"));
        OK(umi_data_server_rollback(server)); OK(umi_data_server_get(server,"note",value,sizeof value)); CHECK(!strcmp(value,"saved"));
    } else if (!strcmp(name,"commit")) {
        OK(umi_data_server_begin(server)); OK(umi_data_server_set(server,"note","saved")); OK(umi_data_server_commit(server));
        OK(umi_data_server_get(server,"note",value,sizeof value)); CHECK(!strcmp(value,"saved"));
    } else if (!strcmp(name,"nested")) {
        OK(umi_data_server_begin(server)); CHECK(umi_data_server_begin(server)==UMI_STATUS_BUSY); OK(umi_data_server_rollback(server));
    } else if (!strcmp(name,"state")) {
        CHECK(!umi_data_server_in_transaction(server)); CHECK(umi_data_server_commit(server)==UMI_STATUS_INVALID_STATE);
        CHECK(umi_data_server_rollback(server)==UMI_STATUS_INVALID_STATE); OK(umi_data_server_begin(server)); CHECK(umi_data_server_in_transaction(server)); OK(umi_data_server_rollback(server));
    } else if (!strcmp(name,"small_read")) {
        OK(umi_data_server_set(server,"note","longer")); char small[2]={'x','x'};
        CHECK(umi_data_server_get(server,"note",small,sizeof small)==UMI_STATUS_CAPACITY_EXCEEDED); CHECK(small[0]==0);
    } else if (!strcmp(name,"missing")) {
        strcpy(value,"previous"); CHECK(umi_data_server_get(server,"absent",value,sizeof value)==UMI_STATUS_NOT_FOUND); CHECK(!value[0]);
        CHECK(umi_data_server_delete(server,"absent")==UMI_STATUS_NOT_FOUND);
    } else if (!strcmp(name,"invalid")) {
        CHECK(umi_data_server_set(NULL,"n","v")==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_data_server_set(server,"","v")==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDataServerCountChecked(NULL,&count)==UMI_STATUS_INVALID_ARGUMENT); CHECK(count==0);
        CHECK(UmiDataServerCopyError(NULL,value,sizeof value)==UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(name,"snapshot")) {
        OK(umi_data_server_set(server,"note","n")); OK(umi_data_server_begin(server));
        UmiDataServerSnapshot snapshot; OK(umi_data_server_snapshot(server,&snapshot)); CHECK(snapshot.record_count==1&&snapshot.transaction_active); OK(umi_data_server_rollback(server));
    } else if (!strcmp(name,"visit")) {
        OK(umi_data_server_set(server,"note","n")); unsigned visits=0; OK(umi_data_server_visit(server,Visitor,&visits)); CHECK(visits==1);
        CHECK(umi_data_server_visit(server,StopVisitor,NULL)==UMI_STATUS_CANCELLED);
    } else if (!strncmp(name,"compete_",8)) {
        int action=atoi(name+8); OK(umi_data_server_set(server,"note","n")); OK(umi_data_server_begin(server));
        Work work={.server=server,.action=action}; CHECK(!Join(&work)); CHECK(work.result==UMI_STATUS_BUSY);
        OK(umi_data_server_rollback(server));
        if (action==0) { CHECK(!Join(&work)); CHECK(work.result==UMI_STATUS_OK); }
    } else if (!strncmp(name,"reenter_",8)) {
        OK(umi_data_server_set(server,"note","n")); Work work={.server=server,.action=atoi(name+8)};
        OK(umi_data_server_visit(server,Reenter,&work)); CHECK(work.result==UMI_STATUS_BUSY);
    } else if (!strcmp(name,"owner_retired")) {
        Work first={.server=server,.action=3}; CHECK(!Join(&first)); CHECK(first.result==UMI_STATUS_OK);
        for (unsigned i=0;i<24;++i) { Work next={.server=server,.action=4}; CHECK(!Join(&next)); CHECK(next.result==UMI_STATUS_BUSY); }
        /* All workers have joined. Destruction, not impersonation or takeover,
         * ends this abandoned test instance after Exercise returns. */
    } else if (!strcmp(name,"repeated")) {
        for (unsigned i=0;i<64;++i) {
            OK(umi_data_server_begin(server)); OK(umi_data_server_set(server,"note","n"));
            if (i%2) OK(umi_data_server_commit(server)); else OK(umi_data_server_rollback(server));
        }
    } else if (!strcmp(name,"two_servers")) {
        UmiDataServer *other=NULL; OK(umi_data_server_create_memory(&other));
        OK(umi_data_server_begin(server)); OK(umi_data_server_begin(other));
        OK(umi_data_server_set(other,"note","other")); OK(umi_data_server_commit(other));
        OK(umi_data_server_rollback(server)); umi_data_server_destroy(other);
    }
#ifdef UMICOM_HAS_SQLITE
    else if (!strcmp(name,"bind_get") || !strcmp(name,"bind_delete") || !strcmp(name,"bind_set")) {
        CHECK(server->backend==UMI_DATA_BACKEND_SQLITE);
        char huge[2048]; memset(huge,'a',sizeof huge-1U); huge[sizeof huge-1U]=0;
        (void)sqlite3_limit(server->sqlite,SQLITE_LIMIT_LENGTH,1024);
        UmiStatus status=!strcmp(name,"bind_get")?umi_data_server_get(server,huge,value,sizeof value):
            !strcmp(name,"bind_delete")?umi_data_server_delete(server,huge):umi_data_server_set(server,huge,"n");
        CHECK(status==UMI_STATUS_CAPACITY_EXCEEDED);
    } else if (!strcmp(name,"rollback_denied")) {
        OK(umi_data_server_begin(server)); OK(umi_data_server_set(server,"note","pending"));
        CHECK(sqlite3_set_authorizer(server->sqlite,DenyTransaction,"ROLLBACK")==SQLITE_OK);
        CHECK(umi_data_server_rollback(server)==UMI_STATUS_IO_ERROR); CHECK(umi_data_server_in_transaction(server));
        CHECK(umi_data_server_set(server,"note","wrong")==UMI_STATUS_INVALID_STATE);
        CHECK(umi_data_server_commit(server)==UMI_STATUS_INVALID_STATE);
        CHECK(sqlite3_set_authorizer(server->sqlite,NULL,NULL)==SQLITE_OK); OK(umi_data_server_rollback(server));
        CHECK(!umi_data_server_in_transaction(server)); CHECK(umi_data_server_get(server,"note",value,sizeof value)==UMI_STATUS_NOT_FOUND);
    } else if (!strcmp(name,"commit_denied")) {
        OK(umi_data_server_begin(server)); OK(umi_data_server_set(server,"note","n"));
        CHECK(sqlite3_set_authorizer(server->sqlite,DenyTransaction,"COMMIT")==SQLITE_OK);
        CHECK(umi_data_server_commit(server)==UMI_STATUS_IO_ERROR); CHECK(umi_data_server_in_transaction(server));
        CHECK(sqlite3_set_authorizer(server->sqlite,NULL,NULL)==SQLITE_OK); OK(umi_data_server_commit(server));
    } else if (!strcmp(name,"automatic_abort")) {
        OK(umi_data_server_execute(server,"CREATE TRIGGER abort_note BEFORE INSERT ON umicom_kv BEGIN SELECT RAISE(ROLLBACK,'injected abort'); END;"));
        OK(umi_data_server_begin(server)); CHECK(umi_data_server_set(server,"note","n")==UMI_STATUS_IO_ERROR);
        CHECK(!umi_data_server_in_transaction(server)); CHECK(umi_data_server_set(server,"note","n")==UMI_STATUS_INVALID_STATE);
        Work work={.server=server,.action=0}; CHECK(!Join(&work)); CHECK(work.result==UMI_STATUS_BUSY);
        CHECK(umi_data_server_rollback(server)==UMI_STATUS_IO_ERROR);
        OK(umi_data_server_execute(server,"DROP TRIGGER abort_note;")); OK(umi_data_server_set(server,"note","n"));
    } else if (!strcmp(name,"raw_transaction")) {
        OK(umi_data_server_execute(server,"BEGIN IMMEDIATE;")); CHECK(umi_data_server_in_transaction(server));
        Work work={.server=server,.action=0}; CHECK(!Join(&work)); CHECK(work.result==UMI_STATUS_BUSY);
        OK(umi_data_server_execute(server,"ROLLBACK;")); CHECK(!umi_data_server_in_transaction(server));
        CHECK(!Join(&work)); CHECK(work.result==UMI_STATUS_OK);
    } else if (!strcmp(name,"count_error")) {
        OK(umi_data_server_execute(server,"DROP TABLE umicom_kv;")); CHECK(UmiDataServerCountChecked(server,&count)==UMI_STATUS_IO_ERROR); CHECK(count==0);
        UmiDataServerSnapshot snapshot; CHECK(umi_data_server_snapshot(server,&snapshot)==UMI_STATUS_IO_ERROR);
    } else if (!strcmp(name,"diagnostic")) {
        CHECK(umi_data_server_execute(server,"not valid SQL")==UMI_STATUS_IO_ERROR);
        char copied[512]; OK(UmiDataServerCopyError(server,copied,sizeof copied)); CHECK(copied[0]);
        const char *legacy=umi_data_server_last_error(server); CHECK(!strcmp(legacy,copied));
        CHECK(umi_data_server_execute(server,"SELECT * FROM missing_table;")==UMI_STATUS_IO_ERROR);
        CHECK(!strcmp(legacy,copied)); char small[1]; CHECK(UmiDataServerCopyError(server,small,sizeof small)==UMI_STATUS_CAPACITY_EXCEEDED); CHECK(!small[0]);
    } else if (!strcmp(name,"embedded_nul")) {
        OK(umi_data_server_execute(server,"INSERT INTO umicom_kv VALUES('note',CAST(x'610062' AS TEXT));"));
        CHECK(umi_data_server_get(server,"note",value,sizeof value)==UMI_STATUS_PARSE_ERROR);
        unsigned visits=0; CHECK(umi_data_server_visit(server,Visitor,&visits)==UMI_STATUS_PARSE_ERROR); CHECK(visits==0);
    }
#endif
    else { fprintf(stderr,"Unknown case: %s\n",name); return 1; }
    return 0;
}
int main(int argc,char **argv)
{
    if (argc!=3) return 2;
    UmiDataServer *server=NULL;
    if (!strcmp(argv[1],"sqlite")) {
#ifndef UMICOM_HAS_SQLITE
        fprintf(stderr,"SKIP: SQLite was deliberately unavailable.\n"); return 77;
#else
        if (umi_data_server_create_sqlite(":memory:",&server)!=UMI_STATUS_OK) return 1;
#endif
    } else if (!strcmp(argv[1],"memory")) {
        if (umi_data_server_create_memory(&server)!=UMI_STATUS_OK) return 1;
    } else return 2;
    int result=Exercise(server,argv[2]); umi_data_server_destroy(server); return result;
}
