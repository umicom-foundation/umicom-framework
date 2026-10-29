/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_text_search.c
 *
 * PURPOSE:
 *   Verify literal body-search results, UTF-8 previews, filters and retained snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "text_fixture.h"
#include "umicom/browser/reading_document.h"
#include "umicom/teacher/learning_reader.h"
#include <stdlib.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);result=1;goto cleanup; } } while (0)
typedef struct CancelState { size_t calls, stop; } CancelState;
static int CancelSearch(void *user)
{
    CancelState *state=user;
    return ++state->calls>=state->stop;
}
static int Check(const char *name,const char *root)
{
    int result=0,known=1;
    UmiLearningShelf *shelf=NULL;
    UmiLearningTextIndex *index=NULL;
    UmiLearningTextSearch *search=NULL;
    UmiBrowserReadingDocument *reading=NULL;
    UmiLearningReader *reader=NULL;
    TextFixture fixture=TextFixtureDefault();
    UmiLearningTextQuery query;
    UmiLearningTextQueryInit(&query); strcpy(query.phrase,"ownership");
    size_t expected=3U;
    UmiStatus expected_status=UMI_STATUS_OK;
    CancelState cancel={0U,1U};
    char preview[300];
    if(strcmp(name,"preview-utf8")==0) {
        memcpy(preview,"ownership ",10U); memset(preview+10U,'X',244U);
        memcpy(preview+254U,"\xe2\x82\xac end",7U); preview[261U]='\0';
        fixture.bytes[1]=preview;fixture.lengths[1]=261U;query.kind=UMI_LEARNING_RESOURCE_EXAMPLE;expected=1U;
    } else if(strcmp(name,"preview-newline")==0) {
        fixture.bytes[1]="ownership\nsecond line";fixture.lengths[1]=strlen(fixture.bytes[1]);
        query.kind=UMI_LEARNING_RESOURCE_EXAMPLE;expected=1U;
    } else if(strcmp(name,"unicode")==0 || strcmp(name,"unicode-case")==0) {
        fixture.bytes[1]="caf\xc3\xa9 ownership";fixture.lengths[1]=strlen(fixture.bytes[1]);
        strcpy(query.phrase,strcmp(name,"unicode")==0 ? "Caf\xc3\xa9" : "CAF\xc3\x89");
        expected=strcmp(name,"unicode")==0 ? 1U : 0U;
    } else if(strcmp(name,"literal")==0) { strcpy(query.phrase,"[abc]");expected=1U; }
    else if(strcmp(name,"hidden-script")==0) { strcpy(query.phrase,"secret-script-token");expected=0U; }
    else if(strcmp(name,"hidden-head")==0) { strcpy(query.phrase,"secret-title-token");expected=0U; }
    else if(strcmp(name,"metadata-only")==0) { strcpy(query.phrase,"Text search examples");expected=0U; }
    else if(strcmp(name,"case-sensitive")==0) { query.case_sensitive=1;expected=2U; }
    else if(strcmp(name,"whole-word")==0) { query.whole_word=1;strcpy(query.phrase,"own");expected=0U; }
    else if(strcmp(name,"substring")==0) strcpy(query.phrase,"own");
    else if(strcmp(name,"product")==0) strcpy(query.product,"studio");
    else if(strcmp(name,"product-case")==0) { strcpy(query.product,"Studio");expected=0U; }
    else if(strcmp(name,"locale")==0) strcpy(query.locale,"EN-gb");
    else if(strcmp(name,"locale-no-fallback")==0) { strcpy(query.locale,"en");expected=0U; }
    else if(strcmp(name,"example")==0) { query.kind=UMI_LEARNING_RESOURCE_EXAMPLE;expected=1U; }
    else if(strcmp(name,"image")==0) { query.kind=UMI_LEARNING_RESOURCE_IMAGE;expected=0U; }
    else if(strcmp(name,"truncated")==0) query.maximum_results=1U;
    else if(strcmp(name,"missing")==0) { strcpy(query.phrase,"absentword");expected=0U; }
    else if(strcmp(name,"empty-query")==0) { query.phrase[0]='\0';expected_status=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(name,"query-limit")==0) { memset(query.phrase,'x',sizeof(query.phrase)-1U);query.phrase[sizeof(query.phrase)-1U]='\0';expected=0U; }
    else if(strcmp(name,"unterminated")==0) { memset(query.phrase,'x',sizeof(query.phrase));expected_status=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(strcmp(name,"product-unterminated")==0) { memset(query.product,'x',sizeof(query.product));expected_status=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(strcmp(name,"locale-unterminated")==0) { memset(query.locale,'x',sizeof(query.locale));expected_status=UMI_STATUS_CAPACITY_EXCEEDED; }
    else if(strcmp(name,"query-control")==0) { strcpy(query.phrase,"own\n");expected_status=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(name,"query-utf8")==0) { query.phrase[0]=(char)0xff;query.phrase[1]='\0';expected_status=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(name,"zero-results")==0) { query.maximum_results=0U;expected_status=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(name,"too-many-results")==0) { query.maximum_results=UMI_LEARNING_TEXT_RESULTS_MAX+1U;expected_status=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(name,"bad-kind")==0) { query.kind=5U;expected_status=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(name,"bad-case")==0) { query.case_sensitive=2;expected_status=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strcmp(name,"bad-word")==0) { query.whole_word=-1;expected_status=UMI_STATUS_INVALID_ARGUMENT; }
    else if(strncmp(name,"cancel-",7U)==0) {
        query.cancellation=CancelSearch;query.cancellation_user=&cancel;expected_status=UMI_STATUS_CANCELLED;
        if(strcmp(name,"cancel-middle")==0)cancel.stop=3U;
        else if(strcmp(name,"cancel-after")==0)cancel.stop=5U;
        else if(strcmp(name,"cancel-before")!=0)known=0;
    } else if(strcmp(name,"two-packs")==0)expected=6U;
    else if(strcmp(name,"default")!=0 && strcmp(name,"order")!=0 && strcmp(name,"projection-offset")!=0 &&
        strcmp(name,"source-offset")!=0 && strcmp(name,"lifetime")!=0 && strcmp(name,"outputs")!=0 &&
        strcmp(name,"arguments")!=0 && strcmp(name,"reopen-changed")!=0 && strcmp(name,"reopen-verified")!=0)
        known=0;
    CHECK(known && UmiLearningShelfCreate(&shelf)==UMI_STATUS_OK);
    CHECK(TextFixtureAdopt(shelf,root,"text.pack",&fixture)==UMI_STATUS_OK);
    if(strcmp(name,"two-packs")==0) CHECK(TextFixtureAdopt(shelf,root,"other.pack",&fixture)==UMI_STATUS_OK);
    CHECK(UmiLearningTextIndexCreate(shelf,NULL,NULL,NULL,&index,NULL)==UMI_STATUS_OK);
    if(strcmp(name,"arguments")==0) {
        CHECK(UmiLearningTextSearchCreate(NULL,&query,&search)==UMI_STATUS_INVALID_ARGUMENT && search==NULL);
        CHECK(UmiLearningTextSearchCreate(index,NULL,&search)==UMI_STATUS_INVALID_ARGUMENT && search==NULL);
        CHECK(UmiLearningTextSearchCreate(index,&query,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        goto cleanup;
    }
    CHECK(UmiLearningTextSearchCreate(index,&query,&search)==expected_status);
    if(expected_status!=UMI_STATUS_OK) { CHECK(search==NULL);goto cleanup; }
    UmiLearningTextSearchInfo info;
    CHECK(UmiLearningTextSearchCopyInfo(search,&info)==UMI_STATUS_OK);
    CHECK(info.matching_documents==expected);
    CHECK(info.result_count==(expected>query.maximum_results ? query.maximum_results : expected));
    CHECK(info.truncated==(expected>query.maximum_results));
    if(strcmp(name,"lifetime")==0) {
        UmiLearningShelfDestroy(shelf);shelf=NULL;
        UmiLearningTextIndexDestroy(index);index=NULL;
    }
    if(expected!=0U) {
        UmiLearningTextHit hit;
        CHECK(UmiLearningTextSearchResultAt(search,0U,&hit)==UMI_STATUS_OK);
        CHECK(hit.byte_end>hit.byte_start && hit.pack.revision==7U);
        if(strcmp(name,"preview-utf8")==0) {
            CHECK(strcmp(hit.resource.id,"source")==0 && strlen(hit.preview)==254U);
            CHECK(hit.preview[253U]=='X' && hit.preview_truncated);
        }
        if(strcmp(name,"preview-newline")==0) CHECK(strcmp(hit.preview,"ownership")==0 && !hit.preview_truncated);
        if(strcmp(name,"order")==0 || strcmp(name,"two-packs")==0) {
            const char *ids[]={"guide","source","note"};
            for(size_t i=0U;i<info.result_count;++i) {
                CHECK(UmiLearningTextSearchResultAt(search,i,&hit)==UMI_STATUS_OK);
                CHECK(strcmp(hit.resource.id,ids[i%3U])==0);
                CHECK(strcmp(hit.pack.id,i<3U ? "text.pack" : "other.pack")==0);
            }
        }
        if(strcmp(name,"projection-offset")==0) {
            CHECK(UmiBrowserReadingDocumentCreate(TextGuide,sizeof(TextGuide)-1U,&reading)==UMI_STATUS_OK);
            const char *text=UmiBrowserReadingDocumentText(reading),*found=strstr(text,"ownership");
            CHECK(found!=NULL && hit.reading_projection && hit.byte_start==(size_t)(found-text));
            CHECK(hit.byte_end-hit.byte_start==9U);
        }
        if(strcmp(name,"source-offset")==0) {
            CHECK(UmiLearningTextSearchResultAt(search,1U,&hit)==UMI_STATUS_OK);
            CHECK(!hit.reading_projection && hit.byte_start==(size_t)(strstr(TextSource,"ownership")-TextSource));
        }
        if(strcmp(name,"reopen-changed")==0 || strcmp(name,"reopen-verified")==0) {
            CHECK(UmiLearningTextSearchResultAt(search,1U,&hit)==UMI_STATUS_OK);
            if(strcmp(name,"reopen-changed")==0) CHECK(UmiRootedFileWrite(root,hit.resource.path,"changed",7U)==UMI_STATUS_OK);
            UmiStatus status=UmiLearningReaderCreateFromShelf(shelf,hit.pack.id,hit.pack.revision,
                hit.resource.id,hit.resource.sha256,UMI_LEARNING_TEXT_SOURCE_MAX,&reader);
            CHECK(status==(strcmp(name,"reopen-changed")==0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK));
        }
    }
    if(strcmp(name,"outputs")==0) {
        UmiLearningTextSearchInfo old;
        memcpy(&old,&info,sizeof(old));
        CHECK(UmiLearningTextSearchCopyInfo(NULL,&info)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&old,&info,sizeof(info))==0);
        CHECK(UmiLearningTextSearchCopyInfo(search,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        UmiLearningTextHit hit,previous;
        memset(&hit,0x5a,sizeof(hit));memcpy(&previous,&hit,sizeof(previous));
        CHECK(UmiLearningTextSearchResultAt(search,99U,&hit)==UMI_STATUS_NOT_FOUND && memcmp(&hit,&previous,sizeof(hit))==0);
        CHECK(UmiLearningTextSearchResultAt(NULL,0U,&hit)==UMI_STATUS_INVALID_ARGUMENT && memcmp(&hit,&previous,sizeof(hit))==0);
        CHECK(UmiLearningTextSearchResultAt(search,0U,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    }
cleanup:
    UmiLearningReaderDestroy(reader);
    UmiBrowserReadingDocumentDestroy(reading);
    UmiLearningTextSearchDestroy(search);
    UmiLearningTextIndexDestroy(index);
    UmiLearningShelfDestroy(shelf);
    return result;
}
int main(int argc,char **argv)
{
    return argc==3 ? Check(argv[1],argv[2]) : 2;
}
