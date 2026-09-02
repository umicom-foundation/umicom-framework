/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_helix_agents.c
 * PURPOSE: Verify the five specialist agents and their capabilities.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/helix/agent_runtime.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiHelixAgentRuntime runtime;
    const UmiHelixRuntimeAgent *agent;
    assert(umi_helix_agent_runtime_register_defaults(&runtime) == UMI_STATUS_OK);
    assert(runtime.count == 5U);
    agent = umi_helix_agent_runtime_find(&runtime, UMI_HELIX_AGENT_ROLE_TEST);
    assert(umi_helix_runtime_agent_has_capability(agent, UMI_HELIX_CAPABILITY_TEST));
    assert(!umi_helix_runtime_agent_has_capability(agent, UMI_HELIX_CAPABILITY_BUILD));
    return 0;
}
