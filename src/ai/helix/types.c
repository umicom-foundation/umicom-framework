/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/helix/types.c
 *
 * PURPOSE:
 *   Initialise bridge data and expose stable bridge text/action helpers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/helix/types.h"
#include <string.h>

void umi_ai_helix_agent_request_init(UmiAiHelixAgentRequest *request)
{
    if (request != NULL) {
        memset(request, 0, sizeof(*request));
    }
}

void umi_ai_helix_agent_result_init(UmiAiHelixAgentResult *result)
{
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
    }
}

const char *umi_ai_helix_disposition_text(UmiAiHelixDisposition disposition)
{
    switch (disposition) {
        case UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION: return "propose_action";
        case UMI_AI_HELIX_DISPOSITION_COMPLETE: return "complete";
        case UMI_AI_HELIX_DISPOSITION_NEEDS_CONTEXT: return "needs_context";
        case UMI_AI_HELIX_DISPOSITION_DEFER: return "defer";
        default: return "unknown";
    }
}

uint32_t umi_ai_helix_action_mask(UmiHelixActionKind kind)
{
    const uint32_t value = (uint32_t)kind;
    if (value < 1U || value > 6U) {
        return 0U;
    }
    return UINT32_C(1) << (value - 1U);
}
