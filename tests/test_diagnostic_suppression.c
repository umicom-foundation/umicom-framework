/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_suppression.c
 *
 * PURPOSE:
 *   Verify auditable diagnostic suppression rules honour scopes, priorities,
 *   expiry, ownership and evidence fields.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "diagnostic_platform_test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDiagnosticSuppressionSet *set = NULL;
    UmiDiagnosticSuppressionRule rule;
    UmiDiagnosticSuppressionDecision decision;
    UmiDiagnosticSuppressionSetSnapshot snapshot;
    UmiDiagnosticProviderFinding finding = test_finding(
        "linter.native", "suppress-1", UMI_DIAGNOSTIC_WARNING, "U100",
        "generated code warning", "generated/model.c", 17U, 3U, 0);

    (void)memset(&rule, 0, sizeof(rule));
    rule.struct_size = (uint32_t)sizeof(rule);
    rule.api_version = UMI_DIAGNOSTIC_SUPPRESSION_API_VERSION;
    test_copy_text(rule.id, sizeof(rule.id), "generated-warning");
    test_copy_text(rule.provider_pattern, sizeof(rule.provider_pattern),
                   "linter.*");
    test_copy_text(rule.uri_pattern, sizeof(rule.uri_pattern), "generated/*");
    test_copy_text(rule.code_pattern, sizeof(rule.code_pattern), "U*");
    test_copy_text(rule.reason, sizeof(rule.reason),
                   "Generated source is verified by its generator tests.");
    test_copy_text(rule.owner, sizeof(rule.owner), "build-engineering");
    rule.priority = 100;
    rule.kind = UMI_DIAGNOSTIC_SUPPRESSION_EXTERNAL;
    rule.status = UMI_DIAGNOSTIC_SUPPRESSION_ACCEPTED;
    rule.enabled = 1;
    rule.expires_timestamp_ns = UINT64_C(5000);

    assert(umi_diagnostic_suppression_set_create(&set) == UMI_STATUS_OK);
    assert(umi_diagnostic_suppression_set_upsert(set, &rule) == UMI_STATUS_OK);
    assert(umi_diagnostic_suppression_set_evaluate(set, &finding, UINT64_C(4000),
                                                   &decision) == UMI_STATUS_OK);
    assert(decision.matched);
    assert(decision.suppressed);
    assert(strcmp(decision.owner, "build-engineering") == 0);

    assert(umi_diagnostic_suppression_set_evaluate(set, &finding, UINT64_C(6000),
                                                   &decision) == UMI_STATUS_OK);
    assert(decision.matched);
    assert(decision.expired);
    assert(!decision.suppressed);
    assert(umi_diagnostic_suppression_set_snapshot(set, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.rule_count == 1U);
    assert(snapshot.accepted_count == 1U);
    assert(snapshot.expiring_count == 1U);

    umi_diagnostic_suppression_set_destroy(set);
    return 0;
}
