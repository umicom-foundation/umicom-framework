/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_privacy_admission.c
 *
 * PURPOSE:
 *   Verify policy/provider domains, preserved valid sharing decisions and atomic service policy refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/ai/authorengine_service.h"

int main(int argc, char **argv)
{
    static const char *const modes[] = {"valid",
        "local-low",
        "local-high",
        "remote-low",
        "remote-high",
        "null-policy",
        "provider-low",
        "provider-high",
        "class-low",
        "class-high",
        "decision-matrix",
        "flags",
        "service-refusal",
        "service-valid"};
    UmiAiPrivacyPolicy policy = umi_ai_privacy_policy_default(), prior_policy;
    UmiAiProviderKind provider = UMI_AI_PROVIDER_LOCAL;
    UmiAiDataClassification classification = UMI_AI_DATA_PUBLIC;
    int known = 0;
    assert(argc == 2);
    for (size_t i = 0U; i < sizeof modes / sizeof modes[0]; ++i)
        if (strcmp(argv[1], modes[i]) == 0) known = 1;
    assert(known);
#define CASE(name) (strcmp(argv[1], name) == 0)
    if (CASE("service-refusal") || CASE("service-valid")) {
        UmiAiRuntime runtime;
        UmiAiAuthorEngineConfig config = {0};
        UmiAiAuthorEngineService *service = NULL;
        UmiAiAuthorEngineServiceSnapshot prior_snapshot, snapshot;
        UmiAiPolicy prior_runtime_policy, requested;
        umi_ai_runtime_init(&runtime);
        strcpy(config.executable, "uaengine"); strcpy(config.workspace, ".");
        strcpy(config.provider, "provider.local");
        assert(umi_ai_authorengine_service_create(&runtime, &config, &service) == UMI_STATUS_OK);
        assert(umi_ai_authorengine_service_snapshot(service, &prior_snapshot) == UMI_STATUS_OK);
        memcpy(&prior_runtime_policy, &runtime.policy, sizeof prior_runtime_policy);
        memcpy(&prior_policy, umi_ai_authorengine_service_privacy_policy(service), sizeof prior_policy);
        requested = runtime.policy; requested.allow_remote = 1;
        if (CASE("service-refusal")) {
            for (unsigned i = 0U; i < 5U; ++i) {
                policy = umi_ai_privacy_policy_default();
                if (i < 2U) policy.maximum_local_classification = (UmiAiDataClassification)(i == 0U ? 0 : 99);
                else policy.maximum_remote_classification = (UmiAiDataClassification)(i == 2U ? 0 : 99);
                assert(umi_ai_authorengine_service_set_policy(service, &requested,
                    i == 4U ? NULL : &policy, 8192U, 1024U) == UMI_STATUS_INVALID_ARGUMENT);
                assert(umi_ai_authorengine_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
                assert(memcmp(&prior_snapshot, &snapshot, sizeof snapshot) == 0);
                assert(memcmp(&prior_runtime_policy, &runtime.policy, sizeof runtime.policy) == 0);
                assert(memcmp(&prior_policy, umi_ai_authorengine_service_privacy_policy(service), sizeof prior_policy) == 0);
            }
        } else {
            policy.maximum_remote_classification = UMI_AI_DATA_INTERNAL;
            policy.persist_sessions = 0;
            assert(umi_ai_authorengine_service_set_policy(service, &requested, &policy, 8192U, 1024U) == UMI_STATUS_OK);
            assert(umi_ai_authorengine_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
            assert(snapshot.revision == prior_snapshot.revision + 1U && snapshot.context_limit == 8192U);
            assert(snapshot.remote_allowed && !snapshot.persistence_allowed);
            assert(memcmp(&policy, umi_ai_authorengine_service_privacy_policy(service), sizeof policy) == 0);
        }
        umi_ai_authorengine_service_destroy(service); umi_ai_runtime_destroy(&runtime);
    } else if (CASE("decision-matrix")) {
        static const UmiStatus expected_without_approval[4][4] = {
            {UMI_STATUS_OK, UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED, UMI_STATUS_PERMISSION_DENIED},
            {UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED, UMI_STATUS_PERMISSION_DENIED, UMI_STATUS_PERMISSION_DENIED},
            {UMI_STATUS_OK, UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED, UMI_STATUS_PERMISSION_DENIED},
            {UMI_STATUS_OK, UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED, UMI_STATUS_PERMISSION_DENIED}
        };
        static const UmiStatus expected_approved[4][4] = {
            {UMI_STATUS_OK, UMI_STATUS_OK, UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED},
            {UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED, UMI_STATUS_PERMISSION_DENIED, UMI_STATUS_PERMISSION_DENIED},
            {UMI_STATUS_OK, UMI_STATUS_OK, UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED},
            {UMI_STATUS_OK, UMI_STATUS_OK, UMI_STATUS_OK, UMI_STATUS_PERMISSION_DENIED}
        };
        for (int p = 1; p <= 4; ++p)
            for (int c = 1; c <= 4; ++c) {
                assert(umi_ai_privacy_policy_check_share(&policy, (UmiAiProviderKind)p,
                    (UmiAiDataClassification)c, 0) == expected_without_approval[p - 1][c - 1]);
                assert(umi_ai_privacy_policy_check_share(&policy, (UmiAiProviderKind)p,
                    (UmiAiDataClassification)c, 1) == expected_approved[p - 1][c - 1]);
            }
        assert(umi_ai_privacy_policy_check_persistence(&policy, UMI_AI_DATA_RESTRICTED, 1) == UMI_STATUS_OK);
        assert(umi_ai_privacy_policy_check_persistence(&policy, UMI_AI_DATA_CONFIDENTIAL, 0) == UMI_STATUS_PERMISSION_DENIED);
    } else if (CASE("flags")) {
        policy.persist_sessions = -1; policy.persist_prompt_text = 2; policy.require_sensitive_approval = -7;
        assert(UmiAiPrivacyPolicyValidate(&policy) == UMI_STATUS_OK);
        assert(umi_ai_privacy_policy_check_persistence(&policy, UMI_AI_DATA_CONFIDENTIAL, 0) == UMI_STATUS_PERMISSION_DENIED);
        assert(umi_ai_privacy_policy_check_persistence(&policy, UMI_AI_DATA_CONFIDENTIAL, -1) == UMI_STATUS_OK);
        policy.persist_prompt_text = 0;
        assert(umi_ai_privacy_policy_check_persistence(&policy, UMI_AI_DATA_PUBLIC, 1) == UMI_STATUS_PERMISSION_DENIED);
        policy.require_sensitive_approval = 0;
        assert(umi_ai_privacy_policy_check_share(&policy, UMI_AI_PROVIDER_LOCAL, UMI_AI_DATA_CONFIDENTIAL, 0) == UMI_STATUS_OK);
    } else {
        if (CASE("local-low")) policy.maximum_local_classification = (UmiAiDataClassification)0;
        if (CASE("local-high")) policy.maximum_local_classification = (UmiAiDataClassification)99;
        if (CASE("remote-low")) policy.maximum_remote_classification = (UmiAiDataClassification)0;
        if (CASE("remote-high")) policy.maximum_remote_classification = (UmiAiDataClassification)99;
        if (CASE("provider-low")) provider = (UmiAiProviderKind)0;
        if (CASE("provider-high")) provider = (UmiAiProviderKind)99;
        if (CASE("class-low")) classification = (UmiAiDataClassification)0;
        if (CASE("class-high")) classification = (UmiAiDataClassification)99;
        const UmiAiPrivacyPolicy *selected = CASE("null-policy") ? NULL : &policy;
        UmiStatus validity = CASE("local-low") || CASE("local-high") || CASE("remote-low") ||
            CASE("remote-high") || CASE("null-policy") ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
        memcpy(&prior_policy, &policy, sizeof policy);
        assert(UmiAiPrivacyPolicyValidate(selected) == validity);
        assert(umi_ai_privacy_policy_check_share(selected, provider, classification, 1) ==
            (CASE("valid") ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT));
        if (validity != UMI_STATUS_OK || CASE("class-low") || CASE("class-high"))
            assert(umi_ai_privacy_policy_check_persistence(selected, classification, 1) == UMI_STATUS_INVALID_ARGUMENT);
        assert(memcmp(&prior_policy, &policy, sizeof policy) == 0);
    }
    return 0;
#undef CASE
}
