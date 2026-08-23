/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_control_risk_policy.c
 *
 * PURPOSE:
 *   Focused integration coverage for AI coding tools control risk policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_coding_tools/catalogue.h"

int main(void)
{
    UmiAiCodingToolPolicy policy;
    const UmiAiCodingToolDescriptor *debug =
        umi_ai_coding_tool_catalogue_find("debug.continue");
    const UmiAiCodingToolDescriptor *agent =
        umi_ai_coding_tool_catalogue_find("agent.apply");
    int approval = 0;

    assert(debug != NULL && agent != NULL);
    umi_ai_coding_tool_policy_init(&policy);

    assert(umi_ai_coding_tool_policy_check(
        &policy, debug, 1, &approval) == UMI_STATUS_PERMISSION_DENIED);

    policy.allowed_capabilities |= UMI_AI_CODING_TOOL_CAP_DEBUG_CONTROL;
    policy.allow_debug_control = 1;
    assert(umi_ai_coding_tool_policy_check(
        &policy, debug, 0, &approval) == UMI_STATUS_PERMISSION_DENIED);
    assert(approval);

    assert(umi_ai_coding_tool_policy_check(
        &policy, agent, 0, &approval) == UMI_STATUS_PERMISSION_DENIED);
    assert(approval);
    return 0;
}

