/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/review_baseline.c
 *
 * PURPOSE:
 *   Collect acknowledged finding fingerprints for regression-aware review.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/review_baseline.h"
#include <string.h>
static void umi_ai_dev_review_baseline_copy(char *dst, size_t cap, const char *src) {
    size_t i = 0U;
    if (dst == NULL || cap == 0U) return;
    if (src != NULL) {
        while (i + 1U < cap && src[i] != '\0') { dst[i] = src[i]; ++i; }
    }
    dst[i] = '\0';
}

void umi_ai_dev_review_baseline_init(UmiAiDevReviewBaseline *collection) { if (collection!=NULL) memset(collection,0,sizeof(*collection)); }
int umi_ai_dev_review_baseline_contains(const UmiAiDevReviewBaseline *collection, const char *id) {
    size_t i;
    if (collection == NULL || id == NULL) {
        return 0;
    }
    for (i = 0U; i < collection->count; ++i) {
        if (strcmp(collection->ids[i], id) == 0) {
            return 1;
        }
    }
    return 0;
}
UmiStatus umi_ai_dev_review_baseline_add(UmiAiDevReviewBaseline *collection, const char *id) { if(collection==NULL||id==NULL||id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; if(umi_ai_dev_review_baseline_contains(collection,id)) return UMI_STATUS_ALREADY_EXISTS; if(collection->count>=UMI_AI_DEV_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; umi_ai_dev_review_baseline_copy(collection->ids[collection->count],UMI_AI_DEV_ID_CAPACITY,id); ++collection->count; ++collection->revision; return UMI_STATUS_OK; }
UmiStatus umi_ai_dev_review_baseline_remove(UmiAiDevReviewBaseline *collection, const char *id) { size_t i,j; if(collection==NULL||id==NULL) return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<collection->count;++i) if(strcmp(collection->ids[i],id)==0) { for(j=i+1U;j<collection->count;++j) memcpy(collection->ids[j-1U],collection->ids[j],UMI_AI_DEV_ID_CAPACITY); --collection->count; ++collection->revision; return UMI_STATUS_OK; } return UMI_STATUS_NOT_FOUND; }
size_t umi_ai_dev_review_baseline_count(const UmiAiDevReviewBaseline *collection) { return collection==NULL?0U:collection->count; }
