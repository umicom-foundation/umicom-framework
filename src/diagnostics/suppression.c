/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/suppression.c
 *
 * PURPOSE:
 *   Implement evidence-preserving diagnostic suppression with wildcard scopes,
 *   fingerprints, source ranges, ownership, review status and expiry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/suppression.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define SUPPRESSION_INITIAL_CAPACITY 16U
#define SUPPRESSION_MAX_CAPACITY 65536U

typedef struct SuppressionEntry {
    UmiDiagnosticSuppressionRule rule;
    uint64_t sequence;
} SuppressionEntry;

struct UmiDiagnosticSuppressionSet {
    SuppressionEntry *items;
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

/* Provide the glob matches operation used by this module and its client applications. */
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
static UmiStatus validate_rule(const UmiDiagnosticSuppressionRule *rule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (rule == NULL || rule->struct_size != (uint32_t)sizeof(*rule) ||
        rule->api_version != UMI_DIAGNOSTIC_SUPPRESSION_API_VERSION ||
        !terminated(rule->id, sizeof(rule->id)) || rule->id[0] == '\0' ||
        !terminated(rule->provider_pattern, sizeof(rule->provider_pattern)) ||
        !terminated(rule->source_pattern, sizeof(rule->source_pattern)) ||
        !terminated(rule->code_pattern, sizeof(rule->code_pattern)) ||
        !terminated(rule->uri_pattern, sizeof(rule->uri_pattern)) ||
        !terminated(rule->tag_pattern, sizeof(rule->tag_pattern)) ||
        !terminated(rule->reason, sizeof(rule->reason)) ||
        !terminated(rule->owner, sizeof(rule->owner)) ||
        rule->reason[0] == '\0' || rule->owner[0] == '\0' ||
        rule->kind < UMI_DIAGNOSTIC_SUPPRESSION_EXTERNAL ||
        rule->kind > UMI_DIAGNOSTIC_SUPPRESSION_POLICY ||
        rule->status < UMI_DIAGNOSTIC_SUPPRESSION_ACCEPTED ||
        rule->status > UMI_DIAGNOSTIC_SUPPRESSION_REJECTED ||
        (rule->end_line != 0U && rule->start_line == 0U) ||
        (rule->end_line != 0U && rule->end_line < rule->start_line)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Provide the find rule operation used by this module and its client applications. */
static size_t find_rule(const UmiDiagnosticSuppressionSet *set,
                        const char *rule_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || rule_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < set->count; ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(set->items[position].rule.id, rule_id) == 0) return position;
    }
    return SIZE_MAX;
}

/* Provide the reserve rules operation used by this module and its client applications. */
static UmiStatus reserve_rules(UmiDiagnosticSuppressionSet *set,
                               size_t required)
{
    size_t capacity;
    SuppressionEntry *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= set->capacity) return UMI_STATUS_OK;
    capacity = set->capacity > 0U
        ? set->capacity : SUPPRESSION_INITIAL_CAPACITY;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity >= SUPPRESSION_MAX_CAPACITY || capacity > SIZE_MAX / 2U) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        capacity *= 2U;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SUPPRESSION_MAX_CAPACITY) {
            capacity = SUPPRESSION_MAX_CAPACITY;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (SuppressionEntry *)realloc(
        set->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    set->items = replacement;
    set->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the rule matches operation used by this module and its client applications. */
static int rule_matches(const UmiDiagnosticSuppressionRule *rule,
                        const UmiDiagnosticProviderFinding *finding)
{
    uint32_t effective_end;
    /* Apply this operation only while the related capability or state is available. */
    if (!rule->enabled) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->fingerprint != 0U && rule->fingerprint != finding->fingerprint) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (rule->start_line != 0U) {
        effective_end = rule->end_line != 0U ? rule->end_line : rule->start_line;
        /* Apply this branch only when its contract condition is satisfied. */
        if (finding->diagnostic.line < rule->start_line ||
            finding->diagnostic.line > effective_end) {
            return 0;
        }
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

/* Provide the outranks operation used by this module and its client applications. */
static int outranks(const SuppressionEntry *candidate,
                    const SuppressionEntry *winner)
{
    return winner == NULL || candidate->rule.priority > winner->rule.priority ||
        (candidate->rule.priority == winner->rule.priority &&
         candidate->sequence > winner->sequence);
}

/*
 * Initialise diagnostic suppression set from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_suppression_set_create(
    UmiDiagnosticSuppressionSet **out_set)
{
    UmiDiagnosticSuppressionSet *set;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_set = NULL;
    set = (UmiDiagnosticSuppressionSet *)calloc(1U, sizeof(*set));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    set->revision = 1U;
    *out_set = set;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by diagnostic suppression set so the same storage can be
 * reused safely.
 */
void umi_diagnostic_suppression_set_destroy(UmiDiagnosticSuppressionSet *set)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return;
    free(set->items);
    set->items = NULL;
    free(set);
}

/*
 * Release or reset state held by diagnostic suppression set so the same storage can be
 * reused safely.
 */
UmiStatus umi_diagnostic_suppression_set_clear(UmiDiagnosticSuppressionSet *set)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    set->count = 0U;
    set->revision = next_revision(set->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic suppression set upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_suppression_set_upsert(
    UmiDiagnosticSuppressionSet *set,
    const UmiDiagnosticSuppressionRule *rule)
{
    size_t position;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || validate_rule(rule) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_rule(set, rule->id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        status = reserve_rules(set, set->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = set->count++;
    }
    set->items[position].rule = *rule;
    set->items[position].rule.enabled = rule->enabled != 0;
    set->items[position].rule.case_sensitive = rule->case_sensitive != 0;
    set->sequence = next_revision(set->sequence);
    set->items[position].sequence = set->sequence;
    set->revision = next_revision(set->revision);
    return UMI_STATUS_OK;
}

/*
 * Remove diagnostic suppression set while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_diagnostic_suppression_set_remove(
    UmiDiagnosticSuppressionSet *set,
    const char *rule_id)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || rule_id == NULL || rule_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_rule(set, rule_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < set->count) {
        (void)memmove(&set->items[position],
                      &set->items[position + 1U],
                      (set->count - position - 1U) * sizeof(*set->items));
    }
    --set->count;
    set->revision = next_revision(set->revision);
    return UMI_STATUS_OK;
}

/*
 * Find diagnostic suppression set while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_suppression_set_at(
    const UmiDiagnosticSuppressionSet *set,
    size_t position,
    UmiDiagnosticSuppressionRule *out_rule)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || out_rule == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= set->count) return UMI_STATUS_NOT_FOUND;
    *out_rule = set->items[position].rule;
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic suppression set evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_suppression_set_evaluate(
    const UmiDiagnosticSuppressionSet *set,
    const UmiDiagnosticProviderFinding *finding,
    uint64_t timestamp_ns,
    UmiDiagnosticSuppressionDecision *out_decision)
{
    const SuppressionEntry *winner = NULL;
    const SuppressionEntry *expired_winner = NULL;
    size_t position;
    int expired;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || finding == NULL || out_decision == NULL ||
        umi_diagnostic_provider_finding_validate(finding) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_decision, 0, sizeof(*out_decision));
    out_decision->struct_size = (uint32_t)sizeof(*out_decision);
    out_decision->api_version = UMI_DIAGNOSTIC_SUPPRESSION_API_VERSION;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < set->count; ++position) {
        const SuppressionEntry *candidate = &set->items[position];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!rule_matches(&candidate->rule, finding)) continue;
        expired = candidate->rule.expires_timestamp_ns != 0U &&
                  timestamp_ns >= candidate->rule.expires_timestamp_ns;
        /* Apply this branch only when its contract condition is satisfied. */
        if (expired) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (outranks(candidate, expired_winner)) expired_winner = candidate;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (outranks(candidate, winner)) {
            winner = candidate;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (winner == NULL) {
        winner = expired_winner;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (winner != NULL) out_decision->expired = 1;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (winner == NULL) return UMI_STATUS_OK;
    out_decision->matched = 1;
    out_decision->kind = winner->rule.kind;
    out_decision->status = winner->rule.status;
    (void)memcpy(out_decision->rule_id, winner->rule.id,
                 strlen(winner->rule.id) + 1U);
    (void)memcpy(out_decision->reason, winner->rule.reason,
                 strlen(winner->rule.reason) + 1U);
    (void)memcpy(out_decision->owner, winner->rule.owner,
                 strlen(winner->rule.owner) + 1U);
    out_decision->suppressed = !out_decision->expired &&
        winner->rule.status == UMI_DIAGNOSTIC_SUPPRESSION_ACCEPTED;
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic suppression set snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_suppression_set_snapshot(
    const UmiDiagnosticSuppressionSet *set,
    UmiDiagnosticSuppressionSetSnapshot *out_snapshot)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DIAGNOSTIC_SUPPRESSION_API_VERSION;
    out_snapshot->rule_count = set->count;
    out_snapshot->revision = set->revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < set->count; ++position) {
        const UmiDiagnosticSuppressionRule *rule = &set->items[position].rule;
        /* Apply this operation only while the related capability or state is available. */
        if (rule->enabled) ++out_snapshot->enabled_count;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (rule->status == UMI_DIAGNOSTIC_SUPPRESSION_ACCEPTED) {
            ++out_snapshot->accepted_count;
        } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (rule->status == UMI_DIAGNOSTIC_SUPPRESSION_UNDER_REVIEW) {
            ++out_snapshot->review_count;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (rule->fingerprint != 0U) ++out_snapshot->fingerprint_count;
        /* Apply this branch only when its contract condition is satisfied. */
        if (rule->expires_timestamp_ns != 0U) ++out_snapshot->expiring_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by diagnostic suppression set without changing
 * their state.
 */
size_t umi_diagnostic_suppression_set_count(
    const UmiDiagnosticSuppressionSet *set)
{
    return set != NULL ? set->count : 0U;
}

/*
 * Provide the diagnostic suppression set revision operation used by this module and its
 * client applications.
 */
uint64_t umi_diagnostic_suppression_set_revision(
    const UmiDiagnosticSuppressionSet *set)
{
    return set != NULL ? set->revision : 0U;
}
