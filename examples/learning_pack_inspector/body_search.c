/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/body_search.c
 *
 * PURPOSE:
 *   Demonstrate verified offline body search using an installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_text.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 4 || argc > 5) {
        fprintf(stderr,"Usage: umicom-learning-pack-text <absolute-root> <manifest> <phrase> [expected-resource-or-none]\n");
        return 2;
    }
    UmiLearningTextQuery query;
    UmiLearningTextQueryInit(&query);
    size_t bytes=strlen(argv[3]);
    if(bytes>=sizeof(query.phrase)) { fprintf(stderr,"Phrase is too long.\n");return 2; }
    memcpy(query.phrase,argv[3],bytes+1U);
    UmiVersion host={UMICOM_FRAMEWORK_VERSION_MAJOR,UMICOM_FRAMEWORK_VERSION_MINOR,UMICOM_FRAMEWORK_VERSION_PATCH};
    UmiLearningPack *pack=NULL;
    UmiLearningShelf *shelf=NULL;
    UmiLearningTextIndex *index=NULL;
    UmiLearningTextSearch *search=NULL;
    UmiLearningTextProblem problem={0};
    /* Check input before any file read. Native frontends call the same validator
     * before transferring a copied query to their worker. */
    UmiStatus status=UmiLearningTextQueryValidate(&query);
    if(status==UMI_STATUS_OK)status=UmiLearningPackLoad(argv[1],argv[2],host,&pack);
    if(status==UMI_STATUS_OK)status=UmiLearningShelfCreate(&shelf);
    if(status==UMI_STATUS_OK)status=UmiLearningShelfAdopt(shelf,&pack);
    /* A graphical application performs this bounded, blocking read on a worker
     * using an independently cloned shelf. No imported example is executed. */
    if(status==UMI_STATUS_OK)status=UmiLearningTextIndexCreate(shelf,NULL,NULL,NULL,&index,&problem);
    if(status==UMI_STATUS_OK)status=UmiLearningTextSearchCreate(index,&query,&search);
    UmiLearningTextSearchInfo info={0};
    if(status==UMI_STATUS_OK)status=UmiLearningTextSearchCopyInfo(search,&info);
    int expected_found=0;
    if(status==UMI_STATUS_OK) {
        printf("Matching documents: %zu; returned: %zu; truncated: %s\n",
            info.matching_documents,info.result_count,info.truncated ? "yes" : "no");
        for(size_t i=0U;status==UMI_STATUS_OK && i<info.result_count;++i) {
            UmiLearningTextHit hit;
            status=UmiLearningTextSearchResultAt(search,i,&hit);
            if(status!=UMI_STATUS_OK)break;
            printf("%s / %s: bytes %zu..%zu (%s)\n  %s%s\n",hit.pack.id,hit.resource.id,
                hit.byte_start,hit.byte_end,hit.reading_projection ? "reading text" : "source text",
                hit.preview,hit.preview_truncated ? " [preview shortened]" : "");
            if(argc==5 && strcmp(argv[4],hit.resource.id)==0)expected_found=1;
        }
        if(argc==5 && strcmp(argv[4],"none")==0)expected_found=info.matching_documents==0U;
        if(status==UMI_STATUS_OK && argc==5 && !expected_found)status=UMI_STATUS_INVALID_STATE;
    }
    if(status!=UMI_STATUS_OK)fprintf(stderr,"Search refused: %s; pack=%s resource=%s\n",
        umi_status_text(status),problem.pack_id,problem.resource_id);
    UmiLearningTextSearchDestroy(search);
    UmiLearningTextIndexDestroy(index);
    UmiLearningShelfDestroy(shelf);
    UmiLearningPackDestroy(pack);
    return status==UMI_STATUS_OK ? 0 : 1;
}
