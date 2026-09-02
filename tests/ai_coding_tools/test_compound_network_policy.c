/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_compound_network_policy.c
 *
 * PURPOSE:
 *   Focused integration coverage for AI coding tools compound network policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_coding_tools/catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiCodingToolPolicy policy;
    const UmiAiCodingToolDescriptor *descriptor =
        umi_ai_coding_tool_catalogue_find("source-control.push");
    int approval = 0;

    assert(descriptor != NULL);
    umi_ai_coding_tool_policy_init(&policy);

    policy.allowed_capabilities |=
        UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_WRITE |
        UMI_AI_CODING_TOOL_CAP_NETWORK;
    policy.allow_network = 1;

    assert(umi_ai_coding_tool_policy_check(
        &policy, descriptor, 1, &approval) == UMI_STATUS_PERMISSION_DENIED);

    policy.allow_source_control_mutation = 1;
    assert(umi_ai_coding_tool_policy_check(
        &policy, descriptor, 0, &approval) == UMI_STATUS_PERMISSION_DENIED);
    assert(approval);

    assert(umi_ai_coding_tool_policy_check(
        &policy, descriptor, 1, &approval) == UMI_STATUS_OK);
    return 0;
}

