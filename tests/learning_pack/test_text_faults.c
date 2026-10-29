/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_text_faults.c
 *
 * PURPOSE:
 *   Inject resource-construction failures and verify candidate ownership is released.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "text_fixture.h"
#include "../../src/teacher/learning_text_internal.h"
#include "umicom/browser/reading_document.h"
#include <stdlib.h>

enum Fault { FAULT_NONE, FAULT_OWNER, FAULT_METADATA, FAULT_EDITOR, FAULT_READING, FAULT_FIRST_DOCUMENT, FAULT_LATER_DOCUMENT };
static enum Fault fault;
static size_t allocations, allocation_calls, editor_owners, reading_owners, byte_owners, upserts;
static void *FaultCalloc(size_t count,size_t size)
{
    ++allocation_calls;
    if((fault==FAULT_OWNER && allocation_calls==1U) ||
       (fault==FAULT_METADATA && allocation_calls==2U)) return NULL;
    void *value=calloc(count,size);
    if(value!=NULL)++allocations;
    return value;
}
static void FaultFree(void *value)
{
    if(value!=NULL) { --allocations;free(value); }
}
static UmiStatus FaultEditorCreate(const UmiEditorWorkspaceSearchIndexConfig *config,UmiEditorWorkspaceSearchIndex **out)
{
    if(fault==FAULT_EDITOR) { *out=NULL;return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status=umi_editor_workspace_search_index_create(config,out);
    if(status==UMI_STATUS_OK)++editor_owners;
    return status;
}
static void FaultEditorDestroy(UmiEditorWorkspaceSearchIndex *index)
{
    if(index!=NULL)--editor_owners;
    umi_editor_workspace_search_index_destroy(index);
}
static UmiStatus FaultUpsert(UmiEditorWorkspaceSearchIndex *index,const UmiEditorWorkspaceSearchDocumentInput *input)
{
    ++upserts;
    if((fault==FAULT_FIRST_DOCUMENT && upserts==1U) ||
       (fault==FAULT_LATER_DOCUMENT && upserts==2U)) return UMI_STATUS_OUT_OF_MEMORY;
    return umi_editor_workspace_search_index_upsert(index,input);
}
static UmiStatus FaultRead(const UmiLearningShelf *shelf,const char *pack,const char *resource,
    size_t maximum,unsigned char **bytes,size_t *size)
{
    UmiStatus status=UmiLearningShelfReadResource(shelf,pack,resource,maximum,bytes,size);
    if(status==UMI_STATUS_OK)++byte_owners;
    return status;
}
static void FaultBytesFree(void *bytes)
{
    if(bytes!=NULL)--byte_owners;
    UmiLearningPackFreeBytes(bytes);
}
static UmiStatus FaultReadingCreate(const void *html,size_t bytes,UmiBrowserReadingDocument **out)
{
    if(fault==FAULT_READING) { *out=NULL;return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status=UmiBrowserReadingDocumentCreate(html,bytes,out);
    if(status==UMI_STATUS_OK)++reading_owners;
    return status;
}
static void FaultReadingDestroy(UmiBrowserReadingDocument *reading)
{
    if(reading!=NULL)--reading_owners;
    UmiBrowserReadingDocumentDestroy(reading);
}
/* Injection is confined to this translation unit. The public library gains no
 * mutable testing hook, allocator switch or process-wide failure behaviour. */
#define calloc FaultCalloc
#define free FaultFree
#define umi_editor_workspace_search_index_create FaultEditorCreate
#define umi_editor_workspace_search_index_destroy FaultEditorDestroy
#define umi_editor_workspace_search_index_upsert FaultUpsert
#define UmiLearningShelfReadResource FaultRead
#define UmiLearningPackFreeBytes FaultBytesFree
#define UmiBrowserReadingDocumentCreate FaultReadingCreate
#define UmiBrowserReadingDocumentDestroy FaultReadingDestroy
#include "../../src/teacher/learning_text_index.c"
#undef calloc
#undef free
#undef umi_editor_workspace_search_index_create
#undef umi_editor_workspace_search_index_destroy
#undef umi_editor_workspace_search_index_upsert
#undef UmiLearningShelfReadResource
#undef UmiLearningPackFreeBytes
#undef UmiBrowserReadingDocumentCreate
#undef UmiBrowserReadingDocumentDestroy

#define CHECK(x) do { if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);result=1;goto cleanup;} } while(0)
static int Check(const char *name,const char *root)
{
    int result=0;
    UmiLearningShelf *shelf=NULL;
    UmiLearningTextIndex *original=NULL,*candidate=NULL;
    UmiLearningTextSearch *search=NULL;
    TextFixture fixture=TextFixtureDefault();
    CHECK(UmiLearningShelfCreate(&shelf)==UMI_STATUS_OK);
    CHECK(TextFixtureAdopt(shelf,root,"text.pack",&fixture)==UMI_STATUS_OK);
    CHECK(UmiLearningTextIndexCreate(shelf,NULL,NULL,NULL,&original,NULL)==UMI_STATUS_OK);
    CHECK(allocations==2U && editor_owners==1U && reading_owners==0U && byte_owners==0U);
    UmiLearningTextIndexInfo before,after;
    CHECK(UmiLearningTextIndexCopyInfo(original,&before)==UMI_STATUS_OK);
    if(strcmp(name,"owner")==0)fault=FAULT_OWNER;
    else if(strcmp(name,"metadata")==0)fault=FAULT_METADATA;
    else if(strcmp(name,"editor")==0)fault=FAULT_EDITOR;
    else if(strcmp(name,"reading")==0)fault=FAULT_READING;
    else if(strcmp(name,"first-document")==0)fault=FAULT_FIRST_DOCUMENT;
    else if(strcmp(name,"later-document")==0)fault=FAULT_LATER_DOCUMENT;
    else CHECK(0);
    allocation_calls=0U;upserts=0U;
    UmiLearningTextProblem problem;
    CHECK(UmiLearningTextIndexCreate(shelf,NULL,NULL,NULL,&candidate,&problem)==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(candidate==NULL && problem.status==UMI_STATUS_OUT_OF_MEMORY);
    CHECK(allocations==2U && editor_owners==1U && reading_owners==0U && byte_owners==0U);
    CHECK(UmiLearningTextIndexCopyInfo(original,&after)==UMI_STATUS_OK && memcmp(&before,&after,sizeof(before))==0);
    if(fault==FAULT_READING || fault==FAULT_FIRST_DOCUMENT) CHECK(strcmp(problem.resource_id,"guide")==0);
    if(fault==FAULT_LATER_DOCUMENT) CHECK(strcmp(problem.resource_id,"source")==0);
    fault=FAULT_NONE;
    UmiLearningTextQuery query;
    UmiLearningTextQueryInit(&query);strcpy(query.phrase,"ownership");
    CHECK(UmiLearningTextSearchCreate(original,&query,&search)==UMI_STATUS_OK);
    UmiLearningTextSearchInfo info;
    CHECK(UmiLearningTextSearchCopyInfo(search,&info)==UMI_STATUS_OK && info.matching_documents==3U);
cleanup:
    fault=FAULT_NONE;
    UmiLearningTextSearchDestroy(search);
    UmiLearningTextIndexDestroy(candidate);
    UmiLearningTextIndexDestroy(original);
    UmiLearningShelfDestroy(shelf);
    if(allocations!=0U || editor_owners!=0U || reading_owners!=0U || byte_owners!=0U) {
        fprintf(stderr,"Retained owners: allocations=%zu editor=%zu reading=%zu bytes=%zu\n",
            allocations,editor_owners,reading_owners,byte_owners);
        result=1;
    }
    return result;
}
int main(int argc,char **argv)
{
    return argc==3 ? Check(argv[1],argv[2]) : 2;
}
