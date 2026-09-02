/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_ai_privacy_policy.c
 *
 * PURPOSE:
 *   Verify conservative local, remote and persistence privacy decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/ai/privacy_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiPrivacyPolicy policy = umi_ai_privacy_policy_default();
    assert(umi_ai_privacy_policy_check_share(
        &policy, UMI_AI_PROVIDER_LOCAL, UMI_AI_DATA_INTERNAL, 0)
        == UMI_STATUS_OK);
    assert(umi_ai_privacy_policy_check_share(
        &policy, UMI_AI_PROVIDER_REMOTE, UMI_AI_DATA_INTERNAL, 1)
        == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ai_privacy_policy_check_share(
        &policy, UMI_AI_PROVIDER_LOCAL, UMI_AI_DATA_CONFIDENTIAL, 0)
        == UMI_STATUS_PERMISSION_DENIED);
    assert(umi_ai_privacy_policy_check_share(
        &policy, UMI_AI_PROVIDER_LOCAL, UMI_AI_DATA_CONFIDENTIAL, 1)
        == UMI_STATUS_OK);
    policy.persist_sessions = 0;
    assert(umi_ai_privacy_policy_check_persistence(
        &policy, UMI_AI_DATA_PUBLIC, 0) == UMI_STATUS_PERMISSION_DENIED);
    return 0;
}
