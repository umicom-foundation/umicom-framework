/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_text_index.c
 *
 * PURPOSE:
 *   Verify atomic verified-resource indexing, explicit budgets and independent ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "text_fixture.h"
#include <stdlib.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result=1; goto cleanup; } } while (0)
typedef struct Cancellation { size_t calls, stop; } Cancellation;
static int Cancel(void *user)
{
    Cancellation *state = user;
    return ++state->calls >= state->stop;
}
static int Check(const char *name, const char *root)
{
    int result = 0;
    UmiLearningShelf *shelf = NULL;
    UmiLearningTextIndex *index = NULL, *earlier = NULL;
    UmiLearningTextSearch *search = NULL;
    UmiLearningTextLimits limits;
    UmiLearningTextLimitsInit(&limits);
    TextFixture fixture = TextFixtureDefault();
    char bad_utf8[] = {(char)0xc0, (char)0xaf};
    char binary[] = {'a', 0, 'b'};
    char control[] = {'a', 0x1b, 'b'};
    char *large = NULL;
    UmiStatus expected = UMI_STATUS_OK;
    const char *expected_resource = "";
    int known = 1;
    if (strcmp(name,"binary")==0 || strcmp(name,"utf8")==0 || strcmp(name,"control")==0) {
        fixture.bytes[1] = strcmp(name,"binary")==0 ? binary : strcmp(name,"utf8")==0 ? bad_utf8 : control;
        fixture.lengths[1] = strcmp(name,"utf8")==0 ? sizeof(bad_utf8) : sizeof(binary);
        expected=UMI_STATUS_INVALID_ARGUMENT; expected_resource="source";
    } else if (strcmp(name,"malformed-html")==0) {
        fixture.bytes[0]="<strong>unfinished"; fixture.lengths[0]=strlen(fixture.bytes[0]);
        expected=UMI_STATUS_INVALID_ARGUMENT; expected_resource="guide";
    } else if (strcmp(name,"empty-documents")==0) {
        for (size_t i=0U;i<3U;++i) { fixture.bytes[i]=""; fixture.lengths[i]=0U; }
    } else if (strcmp(name,"html-case")==0) fixture.paths[0]="z-guide.HTM";
    else if (strcmp(name,"source-limit")==0) { limits.maximum_source_bytes=16U; expected=UMI_STATUS_CAPACITY_EXCEEDED; expected_resource="guide"; }
    else if (strcmp(name,"document-limit")==0) { limits.maximum_document_bytes=8U; expected=UMI_STATUS_CAPACITY_EXCEEDED; expected_resource="guide"; }
    else if (strcmp(name,"total-limit")==0) {
        for (size_t i=0U;i<3U;++i) { fixture.bytes[i]="abcdefgh"; fixture.lengths[i]=8U; }
        fixture.paths[0]="z-guide.txt"; limits.maximum_document_bytes=8U; limits.maximum_total_bytes=16U;
        expected=UMI_STATUS_CAPACITY_EXCEEDED; expected_resource="note";
    } else if (strcmp(name,"total-exact")==0) {
        for (size_t i=0U;i<3U;++i) { fixture.bytes[i]="abcdefgh"; fixture.lengths[i]=8U; }
        fixture.paths[0]="z-guide.txt"; limits.maximum_document_bytes=8U; limits.maximum_total_bytes=24U;
    } else if (strcmp(name,"count-limit")==0) { limits.maximum_documents=2U; expected=UMI_STATUS_CAPACITY_EXCEEDED; expected_resource="note"; }
    else if (strcmp(name,"count-exact")==0) limits.maximum_documents=3U;
    else if (strcmp(name,"maximum-document")==0 || strcmp(name,"oversize-document")==0) {
        size_t length=UMI_LEARNING_TEXT_DOCUMENT_MAX+(strcmp(name,"oversize-document")==0 ? 1U : 0U);
        large=malloc(length); CHECK(large!=NULL); memset(large,'x',length);
        fixture.bytes[1]=large; fixture.lengths[1]=length;
        if (strcmp(name,"oversize-document")==0) { expected=UMI_STATUS_CAPACITY_EXCEEDED; expected_resource="source"; }
    } else if (strcmp(name,"default")!=0 && strcmp(name,"empty-shelf")!=0 &&
        strcmp(name,"changed")!=0 && strcmp(name,"missing")!=0 && strcmp(name,"snapshot")!=0 &&
        strcmp(name,"failed-replacement")!=0 && strcmp(name,"cancel-before")!=0 &&
        strcmp(name,"cancel-middle")!=0 && strcmp(name,"cancel-after")!=0 &&
        strcmp(name,"arguments")!=0 && strcmp(name,"outputs")!=0 && strcmp(name,"two-packs")!=0)
        known=0;
    CHECK(known);
    CHECK(UmiLearningShelfCreate(&shelf)==UMI_STATUS_OK);
    if (strcmp(name,"empty-shelf")!=0) CHECK(TextFixtureAdopt(shelf,root,"text.pack",&fixture)==UMI_STATUS_OK);
    if (strcmp(name,"two-packs")==0) CHECK(TextFixtureAdopt(shelf,root,"other.pack",&fixture)==UMI_STATUS_OK);
    if (strcmp(name,"changed")==0) {
        CHECK(UmiRootedFileWrite(root,fixture.paths[1],"changed",7U)==UMI_STATUS_OK);
        expected=UMI_STATUS_INVALID_STATE; expected_resource="source";
    }
    if (strcmp(name,"missing")==0) {
        CHECK(UmiRootedFileRemove(root,fixture.paths[1])==UMI_STATUS_OK);
        expected=UMI_STATUS_NOT_FOUND; expected_resource="source";
    }
    UmiLearningTextProblem problem;
    memset(&problem,0x5a,sizeof(problem));
    if (strcmp(name,"arguments")==0) {
        CHECK(UmiLearningTextIndexCreate(NULL,&limits,NULL,NULL,&index,&problem)==UMI_STATUS_INVALID_ARGUMENT && index==NULL);
        CHECK(problem.status==UMI_STATUS_INVALID_ARGUMENT && problem.pack_id[0]=='\0');
        CHECK(UmiLearningTextIndexCreate(shelf,&limits,NULL,NULL,NULL,&problem)==UMI_STATUS_INVALID_ARGUMENT);
        for (size_t i=0U;i<8U;++i) {
            UmiLearningTextLimits bad=limits;
            if(i==0U)bad.maximum_documents=0U;
            if(i==1U)bad.maximum_documents=UMI_LEARNING_TEXT_DOCUMENTS_MAX+1U;
            if(i==2U)bad.maximum_source_bytes=0U;
            if(i==3U)bad.maximum_source_bytes=UMI_LEARNING_TEXT_SOURCE_MAX+1U;
            if(i==4U)bad.maximum_document_bytes=0U;
            if(i==5U)bad.maximum_document_bytes=UMI_LEARNING_TEXT_DOCUMENT_MAX+1U;
            if(i==6U)bad.maximum_total_bytes=UMI_LEARNING_TEXT_TOTAL_MAX+1U;
            if(i==7U)bad.maximum_total_bytes=bad.maximum_document_bytes-1U;
            CHECK(UmiLearningTextIndexCreate(shelf,&bad,NULL,NULL,&index,&problem)==UMI_STATUS_INVALID_ARGUMENT && index==NULL);
        }
        goto cleanup;
    }
    Cancellation cancel={0U,1U};
    UmiLearningTextCancellation callback=NULL;
    if (strncmp(name,"cancel-",7U)==0) {
        callback=Cancel;
        if(strcmp(name,"cancel-middle")==0)cancel.stop=3U;
        if(strcmp(name,"cancel-after")==0)cancel.stop=6U;
        expected=UMI_STATUS_CANCELLED;
    }
    CHECK(UmiLearningTextIndexCreate(shelf,&limits,callback,&cancel,&index,&problem)==expected);
    if(expected!=UMI_STATUS_OK) {
        CHECK(index==NULL && problem.status==expected);
        if(expected_resource[0]!='\0') CHECK(strcmp(problem.pack_id,"text.pack")==0 && strcmp(problem.resource_id,expected_resource)==0);
        goto cleanup;
    }
    CHECK(problem.status==UMI_STATUS_OK && problem.pack_id[0]=='\0' && problem.resource_id[0]=='\0');
    UmiLearningTextIndexInfo info;
    CHECK(UmiLearningTextIndexCopyInfo(index,&info)==UMI_STATUS_OK);
    UmiLearningShelfSnapshot snapshot;
    CHECK(UmiLearningShelfCopySnapshot(shelf,&snapshot)==UMI_STATUS_OK && info.shelf_generation==snapshot.generation);
    CHECK(info.document_count==(strcmp(name,"empty-shelf")==0 ? 0U : strcmp(name,"two-packs")==0 ? 6U : 3U));
    CHECK(info.excluded_images==info.pack_count && info.pack_count==snapshot.pack_count);
    if(strcmp(name,"empty-documents")==0) CHECK(info.text_bytes==0U);
    if(strcmp(name,"total-exact")==0) CHECK(info.text_bytes==24U);
    if(strcmp(name,"outputs")==0) {
        UmiLearningTextIndexInfo previous;
        memcpy(&previous,&info,sizeof(previous));
        CHECK(UmiLearningTextIndexCopyInfo(NULL,&info)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&previous,&info,sizeof(info))==0);
        CHECK(UmiLearningTextIndexCopyInfo(index,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }
    if(strcmp(name,"snapshot")==0 || strcmp(name,"failed-replacement")==0) {
        CHECK(UmiRootedFileWrite(root,fixture.paths[1],"changed",7U)==UMI_STATUS_OK);
        if(strcmp(name,"failed-replacement")==0) {
            earlier=index; index=NULL;
            CHECK(UmiLearningTextIndexCreate(shelf,&limits,NULL,NULL,&index,&problem)==UMI_STATUS_INVALID_STATE && index==NULL);
            index=earlier; earlier=NULL;
        }
        UmiLearningShelfDestroy(shelf); shelf=NULL;
        UmiLearningTextQuery query;
        UmiLearningTextQueryInit(&query); strcpy(query.phrase,"ownership");
        CHECK(UmiLearningTextSearchCreate(index,&query,&search)==UMI_STATUS_OK);
        UmiLearningTextSearchInfo found;
        CHECK(UmiLearningTextSearchCopyInfo(search,&found)==UMI_STATUS_OK && found.matching_documents==3U);
    }
cleanup:
    free(large);
    UmiLearningTextSearchDestroy(search);
    UmiLearningTextIndexDestroy(index);
    UmiLearningTextIndexDestroy(earlier);
    UmiLearningShelfDestroy(shelf);
    return result;
}
int main(int argc,char **argv)
{
    return argc==3 ? Check(argv[1],argv[2]) : 2;
}
