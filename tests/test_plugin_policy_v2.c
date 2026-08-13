/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_plugin_policy_v2.c
 *
 * PURPOSE:
 *   Verify trust, signature, permission and isolation decisions.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "plugin_v2_fixture.h"

int main(void)
{
    UmiPluginManifest manifest = plugin_test_manifest("org.example", "Example");
    UmiPluginPolicy policy = umi_plugin_policy_default();
    UmiPluginPermissionDecision permissions;
    UmiPluginSignatureDecision signature;
    UmiPluginPolicyDecision decision;
    (void)memset(&permissions, 0, sizeof(permissions));
    (void)memset(&signature, 0, sizeof(signature));
    assert(umi_plugin_policy_evaluate(&policy, &manifest, &signature,
        &permissions, UMI_PLUGIN_TRUST_LOCAL, &decision) == UMI_STATUS_OK);
    assert(decision.allowed == 1);
    assert(decision.isolation == UMI_PLUGIN_ISOLATION_RESTRICTED_PROCESS);
    policy.require_signature = 1;
    assert(umi_plugin_policy_evaluate(&policy, &manifest, &signature,
        &permissions, UMI_PLUGIN_TRUST_LOCAL, &decision) == UMI_STATUS_PERMISSION_DENIED);
    signature.verified = 1;
    assert(umi_plugin_policy_evaluate(&policy, &manifest, &signature,
        &permissions, UMI_PLUGIN_TRUST_VERIFIED, &decision) == UMI_STATUS_OK);
    assert(decision.isolation == UMI_PLUGIN_ISOLATION_IN_PROCESS);
    assert(strcmp(umi_plugin_trust_level_text(UMI_PLUGIN_TRUST_PLATFORM), "platform") == 0);
    return 0;
}
