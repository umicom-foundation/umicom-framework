/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_types_policy.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI coding tools types policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_tools/policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiCodingToolPolicy policy;
    UmiAiCodingToolDescriptor descriptor = {0};
    int approval = 0;

    umi_ai_coding_tool_policy_init(&policy);
    assert(strcmp(
        umi_ai_coding_tool_risk_text(UMI_AI_CODING_TOOL_RISK_NETWORK),
        "network") == 0);
    assert((policy.allowed_capabilities &
            UMI_AI_CODING_TOOL_CAP_WORKSPACE_READ) != 0U);
    assert(!policy.allow_network);

    (void)strcpy(descriptor.tool_id, "workspace.read");
    descriptor.enabled = 1;
    descriptor.risk = UMI_AI_CODING_TOOL_RISK_READ_ONLY;
    descriptor.required_capabilities =
        UMI_AI_CODING_TOOL_CAP_WORKSPACE_READ;

    assert(umi_ai_coding_tool_policy_check(
        &policy, &descriptor, 0, &approval) == UMI_STATUS_OK);
    assert(!approval);
    return 0;
}

