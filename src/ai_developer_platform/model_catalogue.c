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
/* Full identity and state admission replaces truncating insertion and unchecked traversal; scope-wide revisions reject stale discovery results without changing compatibility entry points. The former implementation is retained for owner review. */
#if 0
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

#endif

/* Model identities are protocol data. Validate their full bounded spelling so
 * discovery cannot merge distinct deployments by silently shortening an ID. */
static int model_id_length(const char *id, size_t *length)
{
    if (id == NULL) return 0;
    for (size_t i = 0U; i < UMI_AI_DEV_ID_CAPACITY; ++i) {
        if (id[i] == '\0') {
            if (i == 0U) return 0;
            if (length != NULL) *length = i;
            return 1;
        }
    }
    return 0;
}

UmiStatus UmiAiDevModelCatalogueValidate(const UmiAiDevModelCatalogue *catalogue)
{
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue->count > UMI_AI_DEV_SMALL_CAPACITY ||
        catalogue->revision < catalogue->count) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < catalogue->count; ++i) {
        if (!model_id_length(catalogue->entries[i].id, NULL))
            return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j) {
            if (strcmp(catalogue->entries[i].id, catalogue->entries[j].id) == 0)
                return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}

void umi_ai_dev_model_catalogue_init(UmiAiDevModelCatalogue *catalogue)
{
    if (catalogue != NULL) memset(catalogue, 0, sizeof(*catalogue));
}

UmiStatus UmiAiDevModelCatalogueUpsertIfRevision(
    UmiAiDevModelCatalogue *catalogue, const char *id,
    uint32_t priority, uint64_t flags, uint64_t expected_revision)
{
    size_t length = 0U;
    UmiStatus status = UmiAiDevModelCatalogueValidate(catalogue);
    if (status != UMI_STATUS_OK) return status;
    if (!model_id_length(id, &length)) return UMI_STATUS_INVALID_ARGUMENT;
    if (catalogue->revision != expected_revision) return UMI_STATUS_BUSY;
    if (catalogue->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

    size_t index = 0U;
    while (index < catalogue->count &&
           strcmp(catalogue->entries[index].id, id) != 0) ++index;
    if (index == catalogue->count && index == UMI_AI_DEV_SMALL_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Capture an aliased ID before clearing a new entry or publishing fields.
     * All refusal paths end above: publication cannot partially modify state. */
    char captured[UMI_AI_DEV_ID_CAPACITY] = {0};
    memcpy(captured, id, length + 1U);
    if (index == catalogue->count) {
        memset(&catalogue->entries[index], 0, sizeof(catalogue->entries[index]));
        memcpy(catalogue->entries[index].id, captured, length + 1U);
        ++catalogue->count;
    }
    catalogue->entries[index].priority = priority;
    catalogue->entries[index].flags = flags;
    catalogue->entries[index].enabled = 1;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_dev_model_catalogue_upsert(UmiAiDevModelCatalogue *catalogue,
    const char *id, uint32_t priority, uint64_t flags)
{
    if (catalogue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiDevModelCatalogueUpsertIfRevision(
        catalogue, id, priority, flags, catalogue->revision);
}

const UmiAiDevModelCatalogueEntry *umi_ai_dev_model_catalogue_find(
    const UmiAiDevModelCatalogue *catalogue, const char *id)
{
    if (UmiAiDevModelCatalogueValidate(catalogue) != UMI_STATUS_OK ||
        !model_id_length(id, NULL)) return NULL;
    for (size_t i = 0U; i < catalogue->count; ++i)
        if (strcmp(catalogue->entries[i].id, id) == 0)
            return &catalogue->entries[i];
    return NULL;
}

size_t umi_ai_dev_model_catalogue_enabled_count(const UmiAiDevModelCatalogue *catalogue)
{
    if (UmiAiDevModelCatalogueValidate(catalogue) != UMI_STATUS_OK) return 0U;
    size_t count = 0U;
    for (size_t i = 0U; i < catalogue->count; ++i)
        if (catalogue->entries[i].enabled != 0) ++count;
    return count;
}
