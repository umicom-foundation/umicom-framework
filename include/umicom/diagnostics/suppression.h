/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/suppression.h
 *
 * PURPOSE:
 *   Define auditable suppression rules for diagnostics. Suppression requires a
 *   reason and owner, can be scoped to provider, rule, path, tag, line range or
 *   fingerprint, and can expire automatically. A suppression never deletes the
 *   underlying finding; Problems and SARIF retain the decision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_SUPPRESSION_H
#define UMICOM_DIAGNOSTICS_SUPPRESSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/diagnostics/diagnostic_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_SUPPRESSION_API_VERSION 1U
#define UMI_DIAGNOSTIC_SUPPRESSION_ID_CAPACITY 128U
#define UMI_DIAGNOSTIC_SUPPRESSION_PATTERN_CAPACITY 512U
#define UMI_DIAGNOSTIC_SUPPRESSION_REASON_CAPACITY 512U
#define UMI_DIAGNOSTIC_SUPPRESSION_OWNER_CAPACITY 128U

/**
 * List the named diagnostic suppression kind values accepted by this public contract.
 */
typedef enum UmiDiagnosticSuppressionKind {
    UMI_DIAGNOSTIC_SUPPRESSION_EXTERNAL = 1,
    UMI_DIAGNOSTIC_SUPPRESSION_IN_SOURCE = 2,
    UMI_DIAGNOSTIC_SUPPRESSION_BASELINE = 3,
    UMI_DIAGNOSTIC_SUPPRESSION_POLICY = 4
} UmiDiagnosticSuppressionKind;

/**
 * List the named diagnostic suppression status values accepted by this public contract.
 */
typedef enum UmiDiagnosticSuppressionStatus {
    UMI_DIAGNOSTIC_SUPPRESSION_ACCEPTED = 1,
    UMI_DIAGNOSTIC_SUPPRESSION_UNDER_REVIEW = 2,
    UMI_DIAGNOSTIC_SUPPRESSION_REJECTED = 3
} UmiDiagnosticSuppressionStatus;

/**
 * Represent the diagnostic suppression rule data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticSuppressionRule {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_DIAGNOSTIC_SUPPRESSION_ID_CAPACITY];
    char provider_pattern[UMI_DIAGNOSTIC_SUPPRESSION_PATTERN_CAPACITY];
    char source_pattern[UMI_DIAGNOSTIC_SUPPRESSION_PATTERN_CAPACITY];
    char code_pattern[UMI_DIAGNOSTIC_SUPPRESSION_PATTERN_CAPACITY];
    char uri_pattern[UMI_DIAGNOSTIC_SUPPRESSION_PATTERN_CAPACITY];
    char tag_pattern[UMI_DIAGNOSTIC_SUPPRESSION_PATTERN_CAPACITY];
    char reason[UMI_DIAGNOSTIC_SUPPRESSION_REASON_CAPACITY];
    char owner[UMI_DIAGNOSTIC_SUPPRESSION_OWNER_CAPACITY];
    uint64_t fingerprint;
    uint64_t expires_timestamp_ns;
    uint32_t start_line;
    uint32_t end_line;
    int32_t priority;
    UmiDiagnosticSuppressionKind kind;
    UmiDiagnosticSuppressionStatus status;
    int case_sensitive;
    int enabled;
} UmiDiagnosticSuppressionRule;

/**
 * Represent the diagnostic suppression decision data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticSuppressionDecision {
    uint32_t struct_size;
    uint32_t api_version;
    char rule_id[UMI_DIAGNOSTIC_SUPPRESSION_ID_CAPACITY];
    char reason[UMI_DIAGNOSTIC_SUPPRESSION_REASON_CAPACITY];
    char owner[UMI_DIAGNOSTIC_SUPPRESSION_OWNER_CAPACITY];
    UmiDiagnosticSuppressionKind kind;
    UmiDiagnosticSuppressionStatus status;
    int matched;
    int suppressed;
    int expired;
} UmiDiagnosticSuppressionDecision;

/**
 * Represent the diagnostic suppression set snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDiagnosticSuppressionSetSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t rule_count;
    size_t enabled_count;
    size_t accepted_count;
    size_t review_count;
    size_t fingerprint_count;
    size_t expiring_count;
    uint64_t revision;
} UmiDiagnosticSuppressionSetSnapshot;

/**
 * Represent the diagnostic suppression set data shared with callers of this public
 * contract.
 */
typedef struct UmiDiagnosticSuppressionSet UmiDiagnosticSuppressionSet;

/**
 * Initialise diagnostic suppression set from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_suppression_set_create(
    UmiDiagnosticSuppressionSet **out_set);
/**
 * Release or reset state held by diagnostic suppression set so the same storage can be
 * reused safely.
 */
void umi_diagnostic_suppression_set_destroy(
    UmiDiagnosticSuppressionSet *set);
/**
 * Release or reset state held by diagnostic suppression set so the same storage can be
 * reused safely.
 */
UmiStatus umi_diagnostic_suppression_set_clear(
    UmiDiagnosticSuppressionSet *set);
/**
 * Provide the diagnostic suppression set upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_suppression_set_upsert(
    UmiDiagnosticSuppressionSet *set,
    const UmiDiagnosticSuppressionRule *rule);
/**
 * Remove diagnostic suppression set while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_diagnostic_suppression_set_remove(
    UmiDiagnosticSuppressionSet *set,
    const char *rule_id);
/**
 * Find diagnostic suppression set while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_suppression_set_at(
    const UmiDiagnosticSuppressionSet *set,
    size_t position,
    UmiDiagnosticSuppressionRule *out_rule);
/**
 * Provide the diagnostic suppression set evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_suppression_set_evaluate(
    const UmiDiagnosticSuppressionSet *set,
    const UmiDiagnosticProviderFinding *finding,
    uint64_t timestamp_ns,
    UmiDiagnosticSuppressionDecision *out_decision);
/**
 * Provide the diagnostic suppression set snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_suppression_set_snapshot(
    const UmiDiagnosticSuppressionSet *set,
    UmiDiagnosticSuppressionSetSnapshot *out_snapshot);
/**
 * Return the number of records represented by diagnostic suppression set without changing
 * their state.
 */
size_t umi_diagnostic_suppression_set_count(
    const UmiDiagnosticSuppressionSet *set);
/**
 * Provide the diagnostic suppression set revision operation used by this module and its
 * client applications.
 */
uint64_t umi_diagnostic_suppression_set_revision(
    const UmiDiagnosticSuppressionSet *set);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DIAGNOSTICS_SUPPRESSION_H */
