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

/*
 * Initialise ai helix agent request from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_helix_agent_request_init(UmiAiHelixAgentRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request != NULL) {
        memset(request, 0, sizeof(*request));
    }
}

/*
 * Initialise ai helix agent result from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_helix_agent_result_init(UmiAiHelixAgentResult *result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result != NULL) {
        memset(result, 0, sizeof(*result));
    }
}

/*
 * Provide the ai helix disposition text operation used by this module and its client
 * applications.
 */
const char *umi_ai_helix_disposition_text(UmiAiHelixDisposition disposition)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (disposition) {
        case UMI_AI_HELIX_DISPOSITION_PROPOSE_ACTION: return "propose_action";
        case UMI_AI_HELIX_DISPOSITION_COMPLETE: return "complete";
        case UMI_AI_HELIX_DISPOSITION_NEEDS_CONTEXT: return "needs_context";
        case UMI_AI_HELIX_DISPOSITION_DEFER: return "defer";
        default: return "unknown";
    }
}

/*
 * Provide the ai helix action mask operation used by this module and its client
 * applications.
 */
uint32_t umi_ai_helix_action_mask(UmiHelixActionKind kind)
{
    const uint32_t value = (uint32_t)kind;
    /* Apply this branch only when its contract condition is satisfied. */
    if (value < 1U || value > 6U) {
        return 0U;
    }
    return UINT32_C(1) << (value - 1U);
}
