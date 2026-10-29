/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/learning_pack_inspector/search.c
 *
 * PURPOSE:
 *   Find offline guide metadata through the shared Framework catalogue service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/teacher/learning_search.h"
#include "umicom/base/text.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 6) {
        fprintf(stderr, "Usage: umicom-learning-pack-search <absolute-pack-root> <manifest> "
            "[query [product [locale]]]\n");
        return 2;
    }
    UmiLearningSearchQuery query = {0};
    UmiStatus status = UMI_STATUS_OK;
    if (argc >= 4) status = umi_text_copy(query.text, sizeof(query.text), argv[3]);
    if (status == UMI_STATUS_OK && argc >= 5)
        status = umi_text_copy(query.product, sizeof(query.product), argv[4]);
    if (status == UMI_STATUS_OK && argc >= 6)
        status = umi_text_copy(query.locale, sizeof(query.locale), argv[5]);
    UmiLearningPack *pack = NULL;
    UmiLearningShelf *shelf = NULL;
    UmiLearningSearch *search = NULL;
    UmiVersion host = {UMICOM_FRAMEWORK_VERSION_MAJOR, UMICOM_FRAMEWORK_VERSION_MINOR,
        UMICOM_FRAMEWORK_VERSION_PATCH};
    if (status == UMI_STATUS_OK) status = UmiLearningPackLoad(argv[1], argv[2], host, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfCreate(&shelf);
    if (status == UMI_STATUS_OK) status = UmiLearningShelfAdopt(shelf, &pack);
    if (status == UMI_STATUS_OK) status = UmiLearningSearchCreate(shelf, &query, 64U, &search);
    /* The result snapshot owns complete metadata, so destroying the catalogue
     * cannot invalidate text later printed or sent to a presentation layer. */
    UmiLearningPackDestroy(pack);
    UmiLearningShelfDestroy(shelf);
    if (status == UMI_STATUS_OK) {
        UmiLearningSearchInfo info;
        status = UmiLearningSearchCopyInfo(search, &info);
        if (status == UMI_STATUS_OK) {
            printf("Matches: %zu; returned: %zu; truncated: %s\n",
                info.matching_resources, info.result_count, info.truncated ? "yes" : "no");
            for (size_t i = 0U; i < info.result_count; ++i) {
                UmiLearningSearchHit hit;
                status = UmiLearningSearchResultAt(search, i, &hit);
                if (status != UMI_STATUS_OK) break;
                printf("%s::%s | %s | %s | %s\n", hit.pack.id, hit.resource.id,
                    hit.resource.title, hit.pack.product, hit.pack.locale);
            }
        }
    }
    UmiLearningSearchDestroy(search);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Catalogue search failed: %s\n", umi_status_text(status));
        return 1;
    }
    return ferror(stdout) ? 1 : 0;
}
