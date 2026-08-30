/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/severity_policy.h
 *
 * PURPOSE:
 *   Apply deterministic workspace and product severity policy to diagnostics.
 *   A compiler warning may remain a warning, become an error in a controlled
 *   workspace, be demoted for generated code, or be disabled by an explicit
 *   rule. Policy is separate from suppression so governance remains visible.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_SEVERITY_POLICY_H
#define UMICOM_DIAGNOSTICS_SEVERITY_POLICY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/diagnostics/diagnostic_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_SEVERITY_POLICY_API_VERSION 1U
#define UMI_DIAGNOSTIC_POLICY_ID_CAPACITY 128U
#define UMI_DIAGNOSTIC_POLICY_PATTERN_CAPACITY 512U

typedef enum UmiDiagnosticSeverityAction {
    UMI_DIAGNOSTIC_SEVERITY_KEEP = 1,
    UMI_DIAGNOSTIC_SEVERITY_SET = 2,
    UMI_DIAGNOSTIC_SEVERITY_PROMOTE = 3,
    UMI_DIAGNOSTIC_SEVERITY_DEMOTE = 4,
    UMI_DIAGNOSTIC_SEVERITY_DISABLE = 5
} UmiDiagnosticSeverityAction;

typedef struct UmiDiagnosticSeverityRule {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DIAGNOSTIC_POLICY_ID_CAPACITY];
    char provider_pattern[UMI_DIAGNOSTIC_POLICY_PATTERN_CAPACITY];
    char source_pattern[UMI_DIAGNOSTIC_POLICY_PATTERN_CAPACITY];
    char code_pattern[UMI_DIAGNOSTIC_POLICY_PATTERN_CAPACITY];
    char uri_pattern[UMI_DIAGNOSTIC_POLICY_PATTERN_CAPACITY];
    char tag_pattern[UMI_DIAGNOSTIC_POLICY_PATTERN_CAPACITY];
    uint32_t kind_mask;
    UmiDiagnosticSeverityAction action;
    UmiDiagnosticSeverity target_severity;
    uint32_t steps;
    int32_t priority;
    int case_sensitive;
    int enabled;
} UmiDiagnosticSeverityRule;

typedef struct UmiDiagnosticSeverityDecision {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDiagnosticSeverity original_severity;
    UmiDiagnosticSeverity effective_severity;
    UmiDiagnosticSeverityAction action;
    char rule_id[UMI_DIAGNOSTIC_POLICY_ID_CAPACITY];
    int matched;
    int changed;
    int disabled;
} UmiDiagnosticSeverityDecision;

typedef struct UmiDiagnosticSeverityPolicySnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t rule_count;
    size_t enabled_count;
    size_t disabling_rule_count;
    size_t promotion_rule_count;
    size_t demotion_rule_count;
    uint64_t revision;
} UmiDiagnosticSeverityPolicySnapshot;

typedef struct UmiDiagnosticSeverityPolicy UmiDiagnosticSeverityPolicy;

UmiStatus umi_diagnostic_severity_policy_create(
    UmiDiagnosticSeverityPolicy **out_policy);
void umi_diagnostic_severity_policy_destroy(
    UmiDiagnosticSeverityPolicy *policy);
UmiStatus umi_diagnostic_severity_policy_clear(
    UmiDiagnosticSeverityPolicy *policy);
UmiStatus umi_diagnostic_severity_policy_upsert(
    UmiDiagnosticSeverityPolicy *policy,
    const UmiDiagnosticSeverityRule *rule);
UmiStatus umi_diagnostic_severity_policy_remove(
    UmiDiagnosticSeverityPolicy *policy,
    const char *rule_id);
UmiStatus umi_diagnostic_severity_policy_at(
    const UmiDiagnosticSeverityPolicy *policy,
    size_t position,
    UmiDiagnosticSeverityRule *out_rule);
UmiStatus umi_diagnostic_severity_policy_evaluate(
    const UmiDiagnosticSeverityPolicy *policy,
    const UmiDiagnosticProviderFinding *finding,
    UmiDiagnosticSeverityDecision *out_decision);
UmiStatus umi_diagnostic_severity_policy_snapshot(
    const UmiDiagnosticSeverityPolicy *policy,
    UmiDiagnosticSeverityPolicySnapshot *out_snapshot);
size_t umi_diagnostic_severity_policy_count(
    const UmiDiagnosticSeverityPolicy *policy);
uint64_t umi_diagnostic_severity_policy_revision(
    const UmiDiagnosticSeverityPolicy *policy);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_SEVERITY_POLICY_H */
