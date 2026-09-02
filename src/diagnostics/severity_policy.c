/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/severity_policy.c
 *
 * PURPOSE:
 *   Implement deterministic wildcard-based severity policy with priority and
 *   latest-rule tie breaking.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/severity_policy.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define POLICY_INITIAL_CAPACITY 16U
#define POLICY_MAX_CAPACITY 65536U

typedef struct SeverityRuleEntry {
    UmiDiagnosticSeverityRule rule;
    uint64_t sequence;
} SeverityRuleEntry;

struct UmiDiagnosticSeverityPolicy {
    SeverityRuleEntry *items;
    size_t count;
    size_t capacity;
    uint64_t sequence;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the character equal operation used by this module and its client applications. */
static int character_equal(unsigned char left,
                           unsigned char right,
                           int case_sensitive)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (!case_sensitive) {
        left = (unsigned char)tolower(left);
        right = (unsigned char)tolower(right);
    }
    return left == right;
}

/* Iterative glob matcher supporting '*' and '?'. It deliberately avoids
 * recursion so policy evaluation remains bounded for long paths. */
static int glob_matches(const char *pattern,
                        const char *text,
                        int case_sensitive)
{
    const char *star = NULL;
    const char *retry = NULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pattern == NULL || pattern[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) text = "";
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*text != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*pattern == '?' ||
            (*pattern != '\0' && *pattern != '*' &&
             character_equal((unsigned char)*pattern,
                             (unsigned char)*text,
                             case_sensitive))) {
            ++pattern;
            ++text;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (*pattern == '*') {
            star = pattern++;
            retry = text;
        } else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (star != NULL) {
            pattern = star + 1;
            text = ++retry;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            return 0;
        }
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*pattern == '*') ++pattern;
    return *pattern == '\0';
}

/* Provide the validate rule operation used by this module and its client applications. */
static UmiStatus validate_rule(const UmiDiagnosticSeverityRule *rule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rule == NULL || rule->struct_size != (uint32_t)sizeof(*rule) ||
        rule->api_version != UMI_DIAGNOSTIC_SEVERITY_POLICY_API_VERSION ||
        !terminated(rule->id, sizeof(rule->id)) || rule->id[0] == '\0' ||
        !terminated(rule->provider_pattern, sizeof(rule->provider_pattern)) ||
        !terminated(rule->source_pattern, sizeof(rule->source_pattern)) ||
        !terminated(rule->code_pattern, sizeof(rule->code_pattern)) ||
        !terminated(rule->uri_pattern, sizeof(rule->uri_pattern)) ||
        !terminated(rule->tag_pattern, sizeof(rule->tag_pattern)) ||
        rule->action < UMI_DIAGNOSTIC_SEVERITY_KEEP ||
        rule->action > UMI_DIAGNOSTIC_SEVERITY_DISABLE ||
        rule->target_severity < UMI_DIAGNOSTIC_TRACE ||
        rule->target_severity > UMI_DIAGNOSTIC_FATAL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((rule->action == UMI_DIAGNOSTIC_SEVERITY_PROMOTE ||
         rule->action == UMI_DIAGNOSTIC_SEVERITY_DEMOTE) &&
        rule->steps == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the find rule operation used by this module and its client applications. */
static size_t find_rule(const UmiDiagnosticSeverityPolicy *policy,
                        const char *rule_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || rule_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < policy->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(policy->items[position].rule.id, rule_id) == 0) {
            return position;
        }
    }
    return SIZE_MAX;
}

/* Provide the reserve rules operation used by this module and its client applications. */
static UmiStatus reserve_rules(UmiDiagnosticSeverityPolicy *policy,
                               size_t required)
{
    size_t capacity;
    SeverityRuleEntry *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= policy->capacity) return UMI_STATUS_OK;
    capacity = policy->capacity > 0U ? policy->capacity : POLICY_INITIAL_CAPACITY;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity >= POLICY_MAX_CAPACITY || capacity > SIZE_MAX / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        capacity *= 2U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > POLICY_MAX_CAPACITY) capacity = POLICY_MAX_CAPACITY;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (SeverityRuleEntry *)realloc(
        policy->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    policy->items = replacement;
    policy->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the rule matches operation used by this module and its client applications. */
static int rule_matches(const UmiDiagnosticSeverityRule *rule,
                        const UmiDiagnosticProviderFinding *finding)
{
    uint32_t kind_bit;
    /* Apply this operation only while the related capability or state is available. */
    if (!rule->enabled) return 0;
    kind_bit = UINT32_C(1) << (uint32_t)finding->diagnostic.kind;
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->kind_mask != 0U && (rule->kind_mask & kind_bit) == 0U) {
        return 0;
    }
    return glob_matches(rule->provider_pattern, finding->provider_id,
                        rule->case_sensitive) &&
           glob_matches(rule->source_pattern, finding->diagnostic.source,
                        rule->case_sensitive) &&
           glob_matches(rule->code_pattern, finding->diagnostic.code,
                        rule->case_sensitive) &&
           glob_matches(rule->uri_pattern, finding->diagnostic.uri,
                        rule->case_sensitive) &&
           glob_matches(rule->tag_pattern, finding->diagnostic.tags,
                        rule->case_sensitive);
}

/* Provide the shifted severity operation used by this module and its client applications. */
static UmiDiagnosticSeverity shifted_severity(UmiDiagnosticSeverity severity,
                                              uint32_t steps,
                                              int direction)
{
    int value = (int)severity;
    int delta = steps > (uint32_t)INT_MAX ? INT_MAX : (int)steps;
    /* Apply this branch only when its contract condition is satisfied. */
    if (direction > 0) {
        value = delta > (int)UMI_DIAGNOSTIC_FATAL - value
            ? (int)UMI_DIAGNOSTIC_FATAL : value + delta;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        value = delta > value - (int)UMI_DIAGNOSTIC_TRACE
            ? (int)UMI_DIAGNOSTIC_TRACE : value - delta;
    }
    return (UmiDiagnosticSeverity)value;
}

/*
 * Initialise diagnostic severity policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_severity_policy_create(
    UmiDiagnosticSeverityPolicy **out_policy)
{
    UmiDiagnosticSeverityPolicy *policy;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_policy = NULL;
    policy = (UmiDiagnosticSeverityPolicy *)calloc(1U, sizeof(*policy));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    policy->revision = 1U;
    *out_policy = policy;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by diagnostic severity policy so the same storage can be
 * reused safely.
 */
void umi_diagnostic_severity_policy_destroy(
    UmiDiagnosticSeverityPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    free(policy->items);
    policy->items = NULL;
    free(policy);
}

/*
 * Release or reset state held by diagnostic severity policy so the same storage can be
 * reused safely.
 */
UmiStatus umi_diagnostic_severity_policy_clear(
    UmiDiagnosticSeverityPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    policy->count = 0U;
    policy->revision = next_revision(policy->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic severity policy upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_severity_policy_upsert(
    UmiDiagnosticSeverityPolicy *policy,
    const UmiDiagnosticSeverityRule *rule)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || validate_rule(rule) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_rule(policy, rule->id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        status = reserve_rules(policy, policy->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = policy->count++;
    }
    policy->items[position].rule = *rule;
    policy->items[position].rule.enabled = rule->enabled != 0;
    policy->items[position].rule.case_sensitive = rule->case_sensitive != 0;
    policy->sequence = next_revision(policy->sequence);
    policy->items[position].sequence = policy->sequence;
    policy->revision = next_revision(policy->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove diagnostic severity policy while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_diagnostic_severity_policy_remove(
    UmiDiagnosticSeverityPolicy *policy,
    const char *rule_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || rule_id == NULL || rule_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_rule(policy, rule_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < policy->count) {
        (void)memmove(&policy->items[position],
                      &policy->items[position + 1U],
                      (policy->count - position - 1U) * sizeof(*policy->items));
    }
    --policy->count;
    policy->revision = next_revision(policy->revision);
    return UMI_STATUS_OK;
}

/*
 * Find diagnostic severity policy while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_severity_policy_at(
    const UmiDiagnosticSeverityPolicy *policy,
    size_t position,
    UmiDiagnosticSeverityRule *out_rule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || out_rule == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= policy->count) return UMI_STATUS_NOT_FOUND;
    *out_rule = policy->items[position].rule;
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic severity policy evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_severity_policy_evaluate(
    const UmiDiagnosticSeverityPolicy *policy,
    const UmiDiagnosticProviderFinding *finding,
    UmiDiagnosticSeverityDecision *out_decision)
{
    const SeverityRuleEntry *winner = NULL;
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || finding == NULL || out_decision == NULL ||
        umi_diagnostic_provider_finding_validate(finding) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_decision, 0, sizeof(*out_decision));
    out_decision->struct_size = (uint32_t)sizeof(*out_decision);
    out_decision->api_version = UMI_DIAGNOSTIC_SEVERITY_POLICY_API_VERSION;
    out_decision->original_severity = finding->diagnostic.severity;
    out_decision->effective_severity = finding->diagnostic.severity;
    out_decision->action = UMI_DIAGNOSTIC_SEVERITY_KEEP;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < policy->count; ++position) {
        const SeverityRuleEntry *candidate = &policy->items[position];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!rule_matches(&candidate->rule, finding)) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (winner == NULL || candidate->rule.priority > winner->rule.priority ||
            (candidate->rule.priority == winner->rule.priority &&
             candidate->sequence > winner->sequence)) {
            winner = candidate;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (winner == NULL) return UMI_STATUS_OK;
    out_decision->matched = 1;
    out_decision->action = winner->rule.action;
    (void)memcpy(out_decision->rule_id, winner->rule.id,
                 strlen(winner->rule.id) + 1U);
    /* Select the behaviour associated with the requested command or state value. */
    switch (winner->rule.action) {
        case UMI_DIAGNOSTIC_SEVERITY_KEEP:
            break;
        case UMI_DIAGNOSTIC_SEVERITY_SET:
            out_decision->effective_severity = winner->rule.target_severity;
            break;
        case UMI_DIAGNOSTIC_SEVERITY_PROMOTE:
            out_decision->effective_severity = shifted_severity(
                finding->diagnostic.severity, winner->rule.steps, 1);
            break;
        case UMI_DIAGNOSTIC_SEVERITY_DEMOTE:
            out_decision->effective_severity = shifted_severity(
                finding->diagnostic.severity, winner->rule.steps, -1);
            break;
        case UMI_DIAGNOSTIC_SEVERITY_DISABLE:
            out_decision->disabled = 1;
            break;
        default:
            return UMI_STATUS_INTERNAL_ERROR;
    }
    out_decision->changed = out_decision->disabled ||
        out_decision->effective_severity != out_decision->original_severity;
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic severity policy snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_severity_policy_snapshot(
    const UmiDiagnosticSeverityPolicy *policy,
    UmiDiagnosticSeverityPolicySnapshot *out_snapshot)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DIAGNOSTIC_SEVERITY_POLICY_API_VERSION;
    out_snapshot->rule_count = policy->count;
    out_snapshot->revision = policy->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < policy->count; ++position) {
        const UmiDiagnosticSeverityRule *rule = &policy->items[position].rule;
        /* Apply this operation only while the related capability or state is available. */
        if (rule->enabled) ++out_snapshot->enabled_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (rule->action == UMI_DIAGNOSTIC_SEVERITY_DISABLE) {
            ++out_snapshot->disabling_rule_count;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (rule->action == UMI_DIAGNOSTIC_SEVERITY_PROMOTE) {
            ++out_snapshot->promotion_rule_count;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (rule->action == UMI_DIAGNOSTIC_SEVERITY_DEMOTE) {
            ++out_snapshot->demotion_rule_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by diagnostic severity policy without changing
 * their state.
 */
size_t umi_diagnostic_severity_policy_count(
    const UmiDiagnosticSeverityPolicy *policy)
{
    return policy != NULL ? policy->count : 0U;
}

/*
 * Provide the diagnostic severity policy revision operation used by this module and its
 * client applications.
 */
uint64_t umi_diagnostic_severity_policy_revision(
    const UmiDiagnosticSeverityPolicy *policy)
{
    return policy != NULL ? policy->revision : 0U;
}
