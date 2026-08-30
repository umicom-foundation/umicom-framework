/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_policy.c
 *
 * PURPOSE:
 *   Verify capability-based allow and deny rules, wildcard matching, and the
 *   secure default-deny behaviour used by tools, modules, plug-ins, and agents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <stdlib.h>

int main(void)
{
    UmiPolicyEngine *policy = NULL;
    UmiPolicyDecision decision;
    UmiPolicyRule allow = {
        "developer", "repository.create", "workspace/*", UMI_POLICY_ALLOW
    };
    UmiPolicyRule deny = {
        "*", "repository.delete", "*", UMI_POLICY_DENY
    };

    if (umi_policy_engine_create(&policy) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    if (umi_policy_engine_add(policy, &allow) != UMI_STATUS_OK ||
        umi_policy_engine_add(policy, &deny) != UMI_STATUS_OK)
        return EXIT_FAILURE;
    decision = umi_policy_engine_authorize(policy,
                                           "developer",
                                           "repository.create",
                                           "workspace/designer");
    if (decision.effect != UMI_POLICY_ALLOW)
        return EXIT_FAILURE;
    decision = umi_policy_engine_authorize(policy,
                                           "developer",
                                           "repository.delete",
                                           "workspace/designer");
    if (decision.effect != UMI_POLICY_DENY)
        return EXIT_FAILURE;
    decision = umi_policy_engine_authorize(policy,
                                           "anonymous",
                                           "repository.create",
                                           "workspace/designer");
    if (decision.effect != UMI_POLICY_DENY)
        return EXIT_FAILURE;
    umi_policy_engine_destroy(policy);
    return EXIT_SUCCESS;
}
