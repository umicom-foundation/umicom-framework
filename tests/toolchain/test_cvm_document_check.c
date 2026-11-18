/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/toolchain/test_cvm_document_check.c
 *
 * PURPOSE:
 *   Verify bounded copies, revision freshness, diagnostic projection and concurrent edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/toolchain/cvm_document_check.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); exit(1); } } while (0)
static const char good[] = "int main(void){\nint unused=7;\nreturn 42;}";
static void Replace(UmiDocumentStore *store,UmiDocumentId id,const char *text)
{
    CHECK(umi_document_store_replace_text(store,id,text,strlen(text)) == UMI_STATUS_OK);
}
typedef struct Worker { UmiDocumentStore *store; UmiDocumentId id; } Worker;
static int Edit(void *context)
{
    Worker *worker=context;
    char large[1025]; memset(large,'x',1024U); large[1024]='\0';
    for(size_t i=0;i<2000U;++i) {
        Replace(worker->store,worker->id,large);
        Replace(worker->store,worker->id,good);
    }
    return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);
    const char *kind=argv[1];
    UmiDocumentStore *store=NULL;
    UmiDocumentId id=0U;
    UmiCvmDocumentCheckResult *r=calloc(1U,sizeof *r);
    UmiDiagnosticSnapshot row,unchanged;
    memset(&row,0x5a,sizeof row); memcpy(&unchanged,&row,sizeof row);
    CHECK(r && umi_document_store_create(&store)==UMI_STATUS_OK);
    CHECK(umi_document_store_new(store,"example.uc",&id)==UMI_STATUS_OK);
    Replace(store,id,good);
    CHECK(umi_cvm_check_document(store,id,SIZE_MAX,r,sizeof *r)==UMI_STATUS_OK);
    CHECK(r->captured && r->source.completed && r->source.report.warning_count==1U);
    CHECK(umi_cvm_document_check_matches(store,r,sizeof *r));
    if(strcmp(kind,"warning")==0) {
        CHECK(umi_cvm_document_check_warning(store,r,sizeof *r,0U,&row)==UMI_STATUS_OK);
        CHECK(row.severity==UMI_DIAGNOSTIC_WARNING && row.kind==UMI_DIAGNOSTIC_KIND_COMPILER);
        CHECK(row.line==2U && row.column==5U && row.end_column==11U);
        CHECK(row.correlation_id==id && row.revision==r->document.revision && row.uri[0]=='\0');
        CHECK(strcmp(row.code,"UMC_LOCAL_NEVER_READ")==0);
        CHECK(umi_cvm_document_check_warning(store,r,sizeof *r,1U,&row)==UMI_STATUS_NOT_FOUND);
        CHECK(umi_cvm_document_check_error(store,r,sizeof *r,&row)==UMI_STATUS_NOT_FOUND);
    } else if(strcmp(kind,"error")==0) {
        Replace(store,id,"int main(void){\nreturn missing;}");
        CHECK(umi_cvm_check_document(store,id,100U,r,sizeof *r)==UMI_STATUS_PARSE_ERROR);
        CHECK(r->captured && !r->source.completed && r->source.report.warning_count==0U);
        CHECK(umi_cvm_document_check_matches(store,r,sizeof *r));
        CHECK(umi_cvm_document_check_error(store,r,sizeof *r,&row)==UMI_STATUS_OK);
        CHECK(row.severity==UMI_DIAGNOSTIC_ERROR && row.line==2U);
        CHECK(umi_cvm_document_check_warning(store,r,sizeof *r,0U,&row)==UMI_STATUS_NOT_FOUND);
    } else if(strcmp(kind,"stale")==0) {
        Replace(store,id,"int main(void){return 42;}");
        CHECK(!umi_cvm_document_check_matches(store,r,sizeof *r));
        CHECK(umi_cvm_document_check_warning(store,r,sizeof *r,0U,&row)==UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&row,&unchanged,sizeof row)==0);
        Replace(store,id,good); /* Identical bytes still have a newer revision. */
        CHECK(!umi_cvm_document_check_matches(store,r,sizeof *r));
        CHECK(umi_cvm_check_document(store,id,100U,r,sizeof *r)==UMI_STATUS_OK);
    } else if(strcmp(kind,"closed")==0) {
        CHECK(umi_document_store_close(store,id,1)==UMI_STATUS_OK);
        CHECK(!umi_cvm_document_check_matches(store,r,sizeof *r));
        CHECK(umi_cvm_check_document(store,id,100U,r,sizeof *r)==UMI_STATUS_NOT_FOUND);
        CHECK(!r->captured && r->source.report.warning_count==0U && r->owner==NULL);
    } else if(strcmp(kind,"isolation")==0) {
        UmiDocumentStore *other=NULL; UmiDocumentId oid=0U;
        CHECK(umi_document_store_create(&other)==UMI_STATUS_OK);
        CHECK(umi_document_store_new(other,"example.uc",&oid)==UMI_STATUS_OK);
        Replace(other,oid,good);
        CHECK(oid==id && !umi_cvm_document_check_matches(other,r,sizeof *r));
        umi_document_store_destroy(other);
    } else if(strcmp(kind,"capacity")==0) {
        CHECK(umi_cvm_check_document(store,id,strlen(good)-1U,r,sizeof *r)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!r->captured && r->source.report.warning_count==0U);
        CHECK(umi_cvm_check_document(store,id,strlen(good),r,sizeof *r)==UMI_STATUS_OK);
        char *large=malloc(UMC_SOURCE_LIMIT+2U); CHECK(large);
        memset(large,' ',UMC_SOURCE_LIMIT+1U); large[UMC_SOURCE_LIMIT+1U]='\0';
        Replace(store,id,large); free(large);
        CHECK(umi_cvm_check_document(store,id,SIZE_MAX,r,sizeof *r)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(!r->captured);
    } else if(strcmp(kind,"arguments")==0) {
        UmiCvmDocumentCheckResult *before=malloc(sizeof *before); CHECK(before);
        memcpy(before,r,sizeof *r);
        CHECK(umi_cvm_check_document(store,id,100U,r,sizeof *r-1U)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(before,r,sizeof *r)==0);
        CHECK(!umi_cvm_document_check_matches(store,r,sizeof *r-1U));
        CHECK(umi_cvm_document_check_error(store,r,sizeof *r,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_cvm_check_document(NULL,id,100U,r,sizeof *r)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!r->captured && r->source.report.warning_count==0U);
        free(before);
    } else if(strcmp(kind,"projection")==0) {
        r->source.report.warnings[0].source_offset=SIZE_MAX;
        CHECK(umi_cvm_document_check_warning(store,r,sizeof *r,0U,&row)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&row,&unchanged,sizeof row)==0);
        CHECK(umi_cvm_check_document(store,id,100U,r,sizeof *r)==UMI_STATUS_OK);
        r->source.report.warnings[0].column=UINT32_MAX;
        CHECK(umi_cvm_document_check_warning(store,r,sizeof *r,0U,&row)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(&row,&unchanged,sizeof row)==0);
    } else if(strcmp(kind,"nonexecution")==0) {
        Replace(store,id,"int main(void){int unused=1/0;return 42;}");
        CHECK(umi_cvm_check_document(store,id,100U,r,sizeof *r)==UMI_STATUS_OK);
        CHECK(r->source.report.warning_count==1U);
    } else if(strcmp(kind,"copy")==0) {
        UmiDocumentSnapshot snapshot,before; memset(&snapshot,0x55,sizeof snapshot);
        memcpy(&before,&snapshot,sizeof snapshot);
        char *copy=NULL;
        CHECK(UmiDocumentStoreCopySnapshotBounded(store,id,0U,&snapshot,&copy)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(copy==NULL && memcmp(&snapshot,&before,sizeof snapshot)==0);
        CHECK(UmiDocumentStoreCopySnapshot(store,id,&snapshot,&copy)==UMI_STATUS_OK);
        Replace(store,id,"");
        CHECK(strcmp(copy,good)==0 && snapshot.length==strlen(good));
        umi_document_store_free_text(copy); copy=NULL;
        CHECK(UmiDocumentStoreCopySnapshotBounded(store,id,0U,&snapshot,&copy)==UMI_STATUS_OK);
        CHECK(snapshot.length==0U && copy[0]=='\0');
        umi_document_store_free_text(copy);
    } else if(strcmp(kind,"concurrent")==0) {
        Worker worker={store,id}; UmiThread *thread=NULL;
        CHECK(umi_thread_start(Edit,&worker,&thread)==UMI_STATUS_OK);
        for(size_t i=0;i<2000U;++i) {
            UmiDocumentSnapshot snapshot; char *copy=NULL;
            UmiStatus status=UmiDocumentStoreCopySnapshotBounded(store,id,100U,&snapshot,&copy);
            CHECK(status==UMI_STATUS_OK || status==UMI_STATUS_CAPACITY_EXCEEDED);
            if(status==UMI_STATUS_OK) {
                CHECK(snapshot.length==strlen(good) && strcmp(copy,good)==0);
                umi_document_store_free_text(copy);
            } else CHECK(copy==NULL);
        }
        int result=-1; CHECK(umi_thread_join(thread,&result)==UMI_STATUS_OK && result==0);
        umi_thread_destroy(thread);
    } else CHECK(false);
    umi_document_store_destroy(store);
    free(r);
    return 0;
}
