/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai/test_provider_kind_policy.c
 *
 * PURPOSE:
 *   Verify provider-kind classification and approval policy for supported and unknown values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../support/assertions.h"
#include <assert.h>
#include <stdint.h>
#include "umicom/ai/provider.h"
#include "umicom/ai/policy.h"

static UmiStatus Generate(void *instance, const UmiAiRequest *request, UmiAiResponse *response)
{
    (void)instance; (void)request; (void)response;
    return UMI_STATUS_OK;
}
int main(void)
{
    UmiAiProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = 1U; provider.provider_id = "test"; provider.generate = Generate;
    const UmiAiProviderKind kinds[] = {
        UMI_AI_PROVIDER_LOCAL, UMI_AI_PROVIDER_REMOTE,
        UMI_AI_PROVIDER_AUTHOR_ENGINE, UMI_AI_PROVIDER_TEST
    };
    const int approvals[] = {0, 1, -3};
    for (size_t i = 0U; i < sizeof(kinds) / sizeof(kinds[0]); ++i) {
        provider.kind = kinds[i];
        assert(umi_ai_provider_validate(&provider) == UMI_STATUS_OK);
        for (int local = 0; local <= 1; ++local) {
            for (int remote = 0; remote <= 1; ++remote) {
                for (int required = 0; required <= 1; ++required) {
                    for (size_t a = 0U; a < sizeof(approvals) / sizeof(approvals[0]); ++a) {
                        UmiAiPolicy policy = umi_ai_policy_default();
                        policy.allow_local = local; policy.allow_remote = remote;
                        policy.require_remote_approval = required;
                        int allowed = kinds[i] == UMI_AI_PROVIDER_REMOTE
                            ? remote && (!required || approvals[a] != 0) : local;
                        assert(umi_ai_policy_check_provider(&policy, kinds[i], approvals[a]) ==
                            (allowed ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED));
                    }
                }
            }
        }
    }
    const UmiAiProviderKind invalid[] = {
        (UmiAiProviderKind)0, (UmiAiProviderKind)-1, (UmiAiProviderKind)5, (UmiAiProviderKind)99
    };
    UmiAiPolicy permissive = {1, 1, 1, 0, 0};
    for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        provider.kind = invalid[i];
        assert(umi_ai_provider_validate(&provider) == UMI_STATUS_INVALID_ARGUMENT);
        assert(umi_ai_policy_check_provider(&permissive, invalid[i], 1) == UMI_STATUS_INVALID_ARGUMENT);
    }
    assert(umi_ai_policy_check_provider(NULL, UMI_AI_PROVIDER_LOCAL, 1) == UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_ai_provider_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    provider.kind = UMI_AI_PROVIDER_LOCAL;
    provider.provider_id = "";
    assert(umi_ai_provider_validate(&provider) == UMI_STATUS_INVALID_ARGUMENT);
    provider.provider_id = "test"; provider.generate = NULL;
    assert(umi_ai_provider_validate(&provider) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
