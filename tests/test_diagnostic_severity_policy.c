/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_severity_policy.c
 *
 * PURPOSE:
 *   Verify ordered severity-policy rules perform deterministic set, promote,
 *   demote, keep and disable decisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "diagnostic_platform_test_support.h"

static UmiDiagnosticSeverityRule rule(const char *id,
                                      const char *code_pattern,
                                      UmiDiagnosticSeverityAction action,
                                      UmiDiagnosticSeverity severity,
                                      uint32_t steps,
                                      int32_t priority)
{
    UmiDiagnosticSeverityRule value;
    (void)memset(&value, 0, sizeof(value));
    value.struct_size = (uint32_t)sizeof(value);
    value.api_version = UMI_DIAGNOSTIC_SEVERITY_POLICY_API_VERSION;
    test_copy_text(value.id, sizeof(value.id), id);
    test_copy_text(value.provider_pattern, sizeof(value.provider_pattern),
                   "compiler.*");
    test_copy_text(value.code_pattern, sizeof(value.code_pattern), code_pattern);
    test_copy_text(value.uri_pattern, sizeof(value.uri_pattern), "src/*");
    value.action = action;
    value.target_severity = severity;
    value.steps = steps;
    value.priority = priority;
    value.enabled = 1;
    return value;
}

int main(void)
{
    UmiDiagnosticSeverityPolicy *policy = NULL;
    UmiDiagnosticSeverityDecision decision;
    UmiDiagnosticSeverityPolicySnapshot snapshot;
    UmiDiagnosticProviderFinding finding = test_finding(
        "compiler.gcc", "policy-1", UMI_DIAGNOSTIC_WARNING, "-Wshadow",
        "shadowing", "src/main.c", 2U, 1U, 0);
    UmiDiagnosticSeverityRule promote = rule(
        "warnings-as-errors", "-W*", UMI_DIAGNOSTIC_SEVERITY_PROMOTE,
        UMI_DIAGNOSTIC_WARNING, 1U, 10);
    UmiDiagnosticSeverityRule disable = rule(
        "disable-shadow", "-Wshadow", UMI_DIAGNOSTIC_SEVERITY_DISABLE,
        UMI_DIAGNOSTIC_WARNING, 0U, 20);

    assert(umi_diagnostic_severity_policy_create(&policy) == UMI_STATUS_OK);
    assert(umi_diagnostic_severity_policy_upsert(policy, &promote) ==
           UMI_STATUS_OK);
    assert(umi_diagnostic_severity_policy_evaluate(policy, &finding,
                                                   &decision) == UMI_STATUS_OK);
    assert(decision.matched);
    assert(decision.effective_severity == UMI_DIAGNOSTIC_ERROR);
    assert(strcmp(decision.rule_id, "warnings-as-errors") == 0);

    assert(umi_diagnostic_severity_policy_upsert(policy, &disable) ==
           UMI_STATUS_OK);
    assert(umi_diagnostic_severity_policy_evaluate(policy, &finding,
                                                   &decision) == UMI_STATUS_OK);
    assert(decision.disabled);
    assert(strcmp(decision.rule_id, "disable-shadow") == 0);
    assert(umi_diagnostic_severity_policy_snapshot(policy, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.rule_count == 2U);
    assert(snapshot.promotion_rule_count == 1U);
    assert(snapshot.disabling_rule_count == 1U);

    umi_diagnostic_severity_policy_destroy(policy);
    return 0;
}
