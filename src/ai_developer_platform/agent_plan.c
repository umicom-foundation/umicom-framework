/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/agent_plan.c
 *
 * PURPOSE:
 *   Collect ordered plan-step identities for governed execution.
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
#include "umicom/ai/developer_platform/agent_plan.h"
#include <string.h>
/*
 * Copy ai dev agent plan into module-owned storage so callers keep ownership of their
 * input values.
 */
static void umi_ai_dev_agent_plan_copy(char *dst, size_t cap, const char *src) {
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
 * Initialise ai dev agent plan from caller-provided values so later operations receive a
 * known state.
 */
void umi_ai_dev_agent_plan_init(UmiAiDevAgentPlan *collection) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (collection!=NULL) memset(collection,0,sizeof(*collection)); }
/*
 * Provide the ai dev agent plan contains operation used by this module and its client
 * applications.
 */
int umi_ai_dev_agent_plan_contains(const UmiAiDevAgentPlan *collection, const char *id) {
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (collection == NULL || id == NULL) {
        return 0;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < collection->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(collection->ids[i], id) == 0) {
            return 1;
        }
    }
    return 0;
}
/* Add ai dev agent plan only after its inputs and available capacity have been checked. */
UmiStatus umi_ai_dev_agent_plan_add(UmiAiDevAgentPlan *collection, const char *id) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(collection==NULL||id==NULL||id[0]=='\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_ai_dev_agent_plan_contains(collection,id)) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(collection->count>=UMI_AI_DEV_MEDIUM_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED; umi_ai_dev_agent_plan_copy(collection->ids[collection->count],UMI_AI_DEV_ID_CAPACITY,id); ++collection->count; ++collection->revision; return UMI_STATUS_OK; }
/*
 * Remove ai dev agent plan while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_ai_dev_agent_plan_remove(UmiAiDevAgentPlan *collection, const char *id) { size_t i,j; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(collection==NULL||id==NULL) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<collection->count;++i) /* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(collection->ids[i],id)==0) { /* Visit each bounded item once so every record receives the same rule. */ for(j=i+1U;j<collection->count;++j) memcpy(collection->ids[j-1U],collection->ids[j],UMI_AI_DEV_ID_CAPACITY); --collection->count; ++collection->revision; return UMI_STATUS_OK; } return UMI_STATUS_NOT_FOUND; }
/*
 * Return the number of records represented by ai dev agent plan without changing their
 * state.
 */
size_t umi_ai_dev_agent_plan_count(const UmiAiDevAgentPlan *collection) { return collection==NULL?0U:collection->count; }
