/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/context_catalogue.c
 *
 * PURPOSE:
 *   Catalogue context sources that can contribute evidence to AI requests.
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
#include "umicom/ai/developer_platform/context_catalogue.h"
#include <string.h>
static void umi_ai_dev_context_catalogue_copy(char *dst, size_t cap, const char *src) {
    size_t i = 0U;
    if (dst == NULL || cap == 0U) return;
    if (src != NULL) {
        while (i + 1U < cap && src[i] != '\0') { dst[i] = src[i]; ++i; }
    }
    dst[i] = '\0';
}

void umi_ai_dev_context_catalogue_init(UmiAiDevContextCatalogue *catalogue) {
    if (catalogue != NULL) {
        memset(catalogue, 0, sizeof(*catalogue));
    }
}
const UmiAiDevContextCatalogueEntry *umi_ai_dev_context_catalogue_find(const UmiAiDevContextCatalogue *catalogue,const char *id) { size_t i; if(catalogue==NULL||id==NULL) return NULL; for(i=0U;i<catalogue->count;++i) if(strcmp(catalogue->entries[i].id,id)==0) return &catalogue->entries[i]; return NULL; }
UmiStatus umi_ai_dev_context_catalogue_upsert(UmiAiDevContextCatalogue *catalogue,const char *id,uint32_t priority,uint64_t flags) { size_t i; if(catalogue==NULL||id==NULL||id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<catalogue->count;++i) if(strcmp(catalogue->entries[i].id,id)==0) { catalogue->entries[i].priority=priority; catalogue->entries[i].flags=flags; catalogue->entries[i].enabled=1; ++catalogue->revision; return UMI_STATUS_OK; } if(catalogue->count>=UMI_AI_DEV_SMALL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; i=catalogue->count++; umi_ai_dev_context_catalogue_copy(catalogue->entries[i].id,UMI_AI_DEV_ID_CAPACITY,id); catalogue->entries[i].priority=priority; catalogue->entries[i].flags=flags; catalogue->entries[i].enabled=1; ++catalogue->revision; return UMI_STATUS_OK; }
size_t umi_ai_dev_context_catalogue_enabled_count(const UmiAiDevContextCatalogue *catalogue) { size_t i,n=0U; if(catalogue==NULL) return 0U; for(i=0U;i<catalogue->count;++i) if(catalogue->entries[i].enabled) ++n; return n; }
