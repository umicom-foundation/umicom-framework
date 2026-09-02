/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/model_catalogue.c
 *
 * PURPOSE:
 *   Maintain the bounded catalogue of AI model endpoints available to developer workflows.
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
#include "umicom/ai/developer_platform/model_catalogue.h"
#include <string.h>
/*
 * Copy ai dev model catalogue into module-owned storage so callers keep ownership of their
 * input values.
 */
static void umi_ai_dev_model_catalogue_copy(char *dst, size_t cap, const char *src) {
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dst == NULL || cap == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (src != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < cap && src[i] != '\0') { dst[i] = src[i]; ++i; }
    }
    dst[i] = '\0';
}

/*
 * Initialise ai dev model catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_dev_model_catalogue_init(UmiAiDevModelCatalogue *catalogue) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue != NULL) {
        memset(catalogue, 0, sizeof(*catalogue));
    }
}
/*
 * Find ai dev model catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiAiDevModelCatalogueEntry *umi_ai_dev_model_catalogue_find(const UmiAiDevModelCatalogue *catalogue,const char *id) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||id==NULL) return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<catalogue->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(catalogue->entries[i].id,id)==0) return &catalogue->entries[i]; return NULL; }
/*
 * Provide the ai dev model catalogue upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_model_catalogue_upsert(UmiAiDevModelCatalogue *catalogue,const char *id,uint32_t priority,uint64_t flags) { size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||id==NULL||id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<catalogue->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(catalogue->entries[i].id,id)==0) { catalogue->entries[i].priority=priority; catalogue->entries[i].flags=flags; catalogue->entries[i].enabled=1; ++catalogue->revision; return UMI_STATUS_OK; } /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue->count>=UMI_AI_DEV_SMALL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; i=catalogue->count++; umi_ai_dev_model_catalogue_copy(catalogue->entries[i].id,UMI_AI_DEV_ID_CAPACITY,id); catalogue->entries[i].priority=priority; catalogue->entries[i].flags=flags; catalogue->entries[i].enabled=1; ++catalogue->revision; return UMI_STATUS_OK; }
/*
 * Return the number of records represented by ai dev model catalogue enabled without
 * changing their state.
 */
size_t umi_ai_dev_model_catalogue_enabled_count(const UmiAiDevModelCatalogue *catalogue) { size_t i,n=0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL) return 0U; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<catalogue->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue->entries[i].enabled) ++n; return n; }
