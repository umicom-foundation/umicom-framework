/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_search_exclusion.c
 *
 * PURPOSE:
 *   Implement deterministic workspace-search resource filtering. The glob
 *   matcher recognises *, **, ?, character classes and path separators without
 *   depending on a platform shell or source-control implementation.
 *
 * RULE PRECEDENCE:
 *   The enabled matching rule with the highest numeric priority wins. When two
 *   matching rules have the same priority, the rule most recently upserted
 *   wins. Built-in hidden/generated/binary policies act as low-priority
 *   implicit exclusions and may therefore be deliberately overridden by an
 *   explicit include rule.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_search_exclusion.h"

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLOB_STEP_LIMIT 1000000ULL
#define IMPLICIT_POLICY_PRIORITY (INT32_MIN + 1)

typedef struct StoredRule {
    UmiEditorWorkspaceSearchExclusionRule rule;
    uint64_t sequence;
} StoredRule;

struct UmiEditorWorkspaceSearchExclusionSet {
    StoredRule *rules;
    size_t count;
    size_t capacity;
    uint64_t revision;
    uint64_t next_sequence;
    int exclude_hidden;
    int exclude_generated;
    int exclude_binary;
};

typedef struct GlobContext {
    const char *pattern;
    const char *text;
    int case_sensitive;
    uint64_t steps;
} GlobContext;

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static void copy_text(char *destination,
                      size_t capacity,
                      const char *source)
{
    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) {
        destination[0] = '\0';
        return;
    }
    (void)snprintf(destination, capacity, "%s", source);
}

static int is_separator(char value)
{
    return value == '/' || value == '\\';
}

static int character_equal(char left, char right, int case_sensitive)
{
    unsigned char left_value = (unsigned char)left;
    unsigned char right_value = (unsigned char)right;
    if (is_separator(left) && is_separator(right)) return 1;
    if (case_sensitive) return left_value == right_value;
    return tolower(left_value) == tolower(right_value);
}

static int class_character_equal(char left,
                                 char right,
                                 int case_sensitive)
{
    unsigned char left_value = (unsigned char)left;
    unsigned char right_value = (unsigned char)right;
    if (case_sensitive) return left_value == right_value;
    return tolower(left_value) == tolower(right_value);
}

static int glob_class_match(const char *pattern,
                            size_t *in_out_position,
                            char text_character,
                            int case_sensitive,
                            int *out_valid)
{
    size_t position = *in_out_position;
    int negated = 0;
    int matched = 0;
    int have_item = 0;
    char previous = '\0';

    *out_valid = 0;
    if (pattern[position] == '!' || pattern[position] == '^') {
        negated = 1;
        ++position;
    }
    while (pattern[position] != '\0') {
        char current = pattern[position++];
        if (current == ']' && have_item) {
            *in_out_position = position;
            *out_valid = 1;
            return negated ? !matched : matched;
        }
        if (current == '-' && have_item &&
            pattern[position] != '\0' && pattern[position] != ']') {
            char range_end = pattern[position++];
            unsigned char value = (unsigned char)text_character;
            unsigned char first = (unsigned char)previous;
            unsigned char last = (unsigned char)range_end;
            if (!case_sensitive) {
                value = (unsigned char)tolower(value);
                first = (unsigned char)tolower(first);
                last = (unsigned char)tolower(last);
            }
            if (first > last) {
                unsigned char temporary = first;
                first = last;
                last = temporary;
            }
            if (value >= first && value <= last) matched = 1;
            have_item = 0;
            continue;
        }
        if (class_character_equal(current,
                                  text_character,
                                  case_sensitive)) {
            matched = 1;
        }
        previous = current;
        have_item = 1;
    }
    return 0;
}

static int glob_match_at(GlobContext *context,
                         size_t pattern_position,
                         size_t text_position)
{
    const char *pattern = context->pattern;
    const char *text = context->text;
    if (++context->steps > GLOB_STEP_LIMIT) return 0;

    while (pattern[pattern_position] != '\0') {
        char token = pattern[pattern_position];
        if (token == '*') {
            int cross_directories = pattern[pattern_position + 1U] == '*';
            size_t next_pattern = pattern_position +
                (cross_directories ? 2U : 1U);
            while (pattern[next_pattern] == '*') ++next_pattern;
            if (is_separator(pattern[next_pattern])) {
                ++next_pattern;
                if (glob_match_at(context, next_pattern, text_position)) {
                    return 1;
                }
                while (text[text_position] != '\0') {
                    if (is_separator(text[text_position])) {
                        if (glob_match_at(context,
                                          next_pattern,
                                          text_position + 1U)) {
                            return 1;
                        }
                    }
                    ++text_position;
                }
                return 0;
            }
            if (glob_match_at(context, next_pattern, text_position)) return 1;
            while (text[text_position] != '\0' &&
                   (cross_directories ||
                    !is_separator(text[text_position]))) {
                ++text_position;
                if (glob_match_at(context,
                                  next_pattern,
                                  text_position)) {
                    return 1;
                }
            }
            return 0;
        }
        if (text[text_position] == '\0') return 0;
        if (token == '?') {
            if (is_separator(text[text_position])) return 0;
            ++pattern_position;
            ++text_position;
            continue;
        }
        if (token == '[') {
            int valid = 0;
            size_t class_position = pattern_position + 1U;
            int matches = glob_class_match(pattern,
                                           &class_position,
                                           text[text_position],
                                           context->case_sensitive,
                                           &valid);
            if (!valid || !matches || is_separator(text[text_position])) {
                return 0;
            }
            pattern_position = class_position;
            ++text_position;
            continue;
        }
        if (!character_equal(token,
                             text[text_position],
                             context->case_sensitive)) {
            return 0;
        }
        ++pattern_position;
        ++text_position;
    }
    return text[text_position] == '\0';
}

static int glob_match(const char *pattern,
                      const char *text,
                      int case_sensitive)
{
    GlobContext context;
    if (pattern == NULL || text == NULL) return 0;
    (void)memset(&context, 0, sizeof(context));
    context.pattern = pattern;
    context.text = text;
    context.case_sensitive = case_sensitive;
    return glob_match_at(&context, 0U, 0U);
}

static int literal_match(const char *pattern,
                         const char *text,
                         int case_sensitive)
{
    size_t index;
    if (pattern == NULL || text == NULL) return 0;
    for (index = 0U; pattern[index] != '\0' && text[index] != '\0'; ++index) {
        if (!character_equal(pattern[index], text[index], case_sensitive)) {
            return 0;
        }
    }
    return pattern[index] == '\0' && text[index] == '\0';
}

static int rule_valid(const UmiEditorWorkspaceSearchExclusionRule *rule)
{
    return rule != NULL &&
           rule->struct_size == (uint32_t)sizeof(*rule) &&
           rule->api_version ==
               UMI_EDITOR_WORKSPACE_SEARCH_EXCLUSION_API_VERSION &&
           terminated(rule->id, sizeof(rule->id)) && rule->id[0] != '\0' &&
           terminated(rule->pattern, sizeof(rule->pattern)) &&
           rule->pattern[0] != '\0' &&
           terminated(rule->reason, sizeof(rule->reason)) &&
           rule->action >= UMI_EDITOR_WORKSPACE_SEARCH_RULE_INCLUDE &&
           rule->action <= UMI_EDITOR_WORKSPACE_SEARCH_RULE_EXCLUDE &&
           rule->target >= UMI_EDITOR_WORKSPACE_SEARCH_RULE_PATH &&
           rule->target <= UMI_EDITOR_WORKSPACE_SEARCH_RULE_LANGUAGE &&
           rule->syntax >= UMI_EDITOR_WORKSPACE_SEARCH_RULE_LITERAL &&
           rule->syntax <= UMI_EDITOR_WORKSPACE_SEARCH_RULE_GLOB;
}

static int facts_valid(const UmiEditorWorkspaceSearchResourceFacts *facts)
{
    return facts != NULL &&
           facts->struct_size == (uint32_t)sizeof(*facts) &&
           facts->api_version ==
               UMI_EDITOR_WORKSPACE_SEARCH_EXCLUSION_API_VERSION &&
           facts->uri != NULL && facts->uri[0] != '\0';
}

static UmiStatus reserve_rules(UmiEditorWorkspaceSearchExclusionSet *set,
                               size_t required)
{
    StoredRule *replacement;
    size_t capacity;
    if (required <= set->capacity) return UMI_STATUS_OK;
    capacity = set->capacity == 0U ? 16U : set->capacity * 2U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (StoredRule *)realloc(
        set->rules, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    set->rules = replacement;
    set->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_rule(const UmiEditorWorkspaceSearchExclusionSet *set,
                        const char *rule_id)
{
    size_t index;
    if (set == NULL || rule_id == NULL) return SIZE_MAX;
    for (index = 0U; index < set->count; ++index) {
        if (strcmp(set->rules[index].rule.id, rule_id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_editor_workspace_search_exclusion_set_create(
    UmiEditorWorkspaceSearchExclusionSet **out_set)
{
    UmiEditorWorkspaceSearchExclusionSet *set;
    if (out_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_set = NULL;
    set = (UmiEditorWorkspaceSearchExclusionSet *)calloc(1U, sizeof(*set));
    if (set == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    set->revision = 1U;
    set->next_sequence = 1U;
    set->exclude_hidden = 1;
    set->exclude_generated = 1;
    set->exclude_binary = 1;
    *out_set = set;
    return UMI_STATUS_OK;
}

void umi_editor_workspace_search_exclusion_set_destroy(
    UmiEditorWorkspaceSearchExclusionSet *set)
{
    if (set == NULL) return;
    free(set->rules);
    set->rules = NULL;
    free(set);
}

UmiStatus umi_editor_workspace_search_exclusion_set_clear(
    UmiEditorWorkspaceSearchExclusionSet *set)
{
    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    set->count = 0U;
    set->revision = next_revision(set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_exclusion_set_policy(
    UmiEditorWorkspaceSearchExclusionSet *set,
    int exclude_hidden,
    int exclude_generated,
    int exclude_binary)
{
    if (set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    set->exclude_hidden = exclude_hidden != 0;
    set->exclude_generated = exclude_generated != 0;
    set->exclude_binary = exclude_binary != 0;
    set->revision = next_revision(set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_exclusion_set_upsert(
    UmiEditorWorkspaceSearchExclusionSet *set,
    const UmiEditorWorkspaceSearchExclusionRule *rule)
{
    size_t index;
    UmiStatus status;
    if (set == NULL || !rule_valid(rule)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_rule(set, rule->id);
    if (index == SIZE_MAX) {
        status = reserve_rules(set, set->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = set->count++;
    }
    set->rules[index].rule = *rule;
    set->rules[index].sequence = set->next_sequence++;
    if (set->next_sequence == 0U) set->next_sequence = 1U;
    set->revision = next_revision(set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_exclusion_set_remove(
    UmiEditorWorkspaceSearchExclusionSet *set,
    const char *rule_id)
{
    size_t index;
    if (set == NULL || rule_id == NULL || rule_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_rule(set, rule_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < set->count) {
        (void)memmove(&set->rules[index],
                      &set->rules[index + 1U],
                      (set->count - index - 1U) * sizeof(*set->rules));
    }
    --set->count;
    set->revision = next_revision(set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_exclusion_set_at(
    const UmiEditorWorkspaceSearchExclusionSet *set,
    size_t index,
    UmiEditorWorkspaceSearchExclusionRule *out_rule)
{
    if (set == NULL || out_rule == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= set->count) return UMI_STATUS_NOT_FOUND;
    *out_rule = set->rules[index].rule;
    return UMI_STATUS_OK;
}

static const char *path_file_name(const char *path)
{
    const char *cursor;
    const char *name;
    if (path == NULL) return "";
    name = path;
    for (cursor = path; *cursor != '\0'; ++cursor) {
        if (is_separator(*cursor)) name = cursor + 1;
    }
    return name;
}

static const char *facts_value(const UmiEditorWorkspaceSearchResourceFacts *facts,
                               UmiEditorWorkspaceSearchRuleTarget target)
{
    switch (target) {
        case UMI_EDITOR_WORKSPACE_SEARCH_RULE_PATH:
            if (facts->relative_path != NULL && facts->relative_path[0] != '\0') {
                return facts->relative_path;
            }
            return facts->uri;
        case UMI_EDITOR_WORKSPACE_SEARCH_RULE_FILE_NAME:
            if (facts->file_name != NULL && facts->file_name[0] != '\0') {
                return facts->file_name;
            }
            return path_file_name(facts->relative_path != NULL
                ? facts->relative_path : facts->uri);
        case UMI_EDITOR_WORKSPACE_SEARCH_RULE_EXTENSION:
            return facts->extension != NULL ? facts->extension : "";
        case UMI_EDITOR_WORKSPACE_SEARCH_RULE_LANGUAGE:
            return facts->language_id != NULL ? facts->language_id : "";
        default:
            return "";
    }
}

static int rule_matches(const UmiEditorWorkspaceSearchExclusionRule *rule,
                        const UmiEditorWorkspaceSearchResourceFacts *facts)
{
    const char *value = facts_value(facts, rule->target);
    return rule->syntax == UMI_EDITOR_WORKSPACE_SEARCH_RULE_GLOB
        ? glob_match(rule->pattern, value, rule->case_sensitive)
        : literal_match(rule->pattern, value, rule->case_sensitive);
}

static void decision_implicit(
    UmiEditorWorkspaceSearchExclusionDecision *decision,
    UmiEditorWorkspaceSearchExclusionReason reason,
    const char *explanation)
{
    decision->included = 0;
    decision->reason = reason;
    decision->matching_priority = IMPLICIT_POLICY_PRIORITY;
    copy_text(decision->explanation,
              sizeof(decision->explanation),
              explanation);
}

UmiStatus umi_editor_workspace_search_exclusion_evaluate(
    const UmiEditorWorkspaceSearchExclusionSet *set,
    const UmiEditorWorkspaceSearchResourceFacts *facts,
    UmiEditorWorkspaceSearchExclusionDecision *out_decision)
{
    size_t index;
    int32_t best_priority = INT32_MIN;
    uint64_t best_sequence = 0U;
    const StoredRule *best_rule = NULL;

    if (set == NULL || !facts_valid(facts) || out_decision == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_decision, 0, sizeof(*out_decision));
    out_decision->struct_size = (uint32_t)sizeof(*out_decision);
    out_decision->api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_EXCLUSION_API_VERSION;
    out_decision->included = 1;
    out_decision->reason = UMI_EDITOR_WORKSPACE_SEARCH_INCLUDED;
    out_decision->matching_priority = INT32_MIN;
    out_decision->rule_set_revision = set->revision;

    if (set->exclude_hidden && facts->hidden) {
        decision_implicit(out_decision,
                          UMI_EDITOR_WORKSPACE_SEARCH_EXCLUDED_HIDDEN,
                          "Hidden resources are excluded by workspace policy");
        best_priority = IMPLICIT_POLICY_PRIORITY;
    }
    if (set->exclude_generated && facts->generated) {
        decision_implicit(out_decision,
                          UMI_EDITOR_WORKSPACE_SEARCH_EXCLUDED_GENERATED,
                          "Generated resources are excluded by workspace policy");
        best_priority = IMPLICIT_POLICY_PRIORITY;
    }
    if (set->exclude_binary && facts->binary) {
        decision_implicit(out_decision,
                          UMI_EDITOR_WORKSPACE_SEARCH_EXCLUDED_BINARY,
                          "Binary resources are excluded by workspace policy");
        best_priority = IMPLICIT_POLICY_PRIORITY;
    }

    for (index = 0U; index < set->count; ++index) {
        const StoredRule *candidate = &set->rules[index];
        if (!candidate->rule.enabled ||
            !rule_matches(&candidate->rule, facts)) {
            continue;
        }
        if (candidate->rule.priority > best_priority ||
            (candidate->rule.priority == best_priority &&
             candidate->sequence > best_sequence)) {
            best_rule = candidate;
            best_priority = candidate->rule.priority;
            best_sequence = candidate->sequence;
        }
    }

    if (best_rule != NULL) {
        out_decision->included =
            best_rule->rule.action == UMI_EDITOR_WORKSPACE_SEARCH_RULE_INCLUDE;
        out_decision->reason = out_decision->included
            ? UMI_EDITOR_WORKSPACE_SEARCH_INCLUDED
            : UMI_EDITOR_WORKSPACE_SEARCH_EXCLUDED_BY_RULE;
        out_decision->matching_priority = best_rule->rule.priority;
        copy_text(out_decision->matching_rule_id,
                  sizeof(out_decision->matching_rule_id),
                  best_rule->rule.id);
        copy_text(out_decision->explanation,
                  sizeof(out_decision->explanation),
                  best_rule->rule.reason[0] != '\0'
                      ? best_rule->rule.reason
                      : (out_decision->included
                         ? "Resource included by an explicit search rule"
                         : "Resource excluded by an explicit search rule"));
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_search_exclusion_snapshot(
    const UmiEditorWorkspaceSearchExclusionSet *set,
    UmiEditorWorkspaceSearchExclusionSnapshot *out_snapshot)
{
    size_t index;
    if (set == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_WORKSPACE_SEARCH_EXCLUSION_API_VERSION;
    out_snapshot->rule_count = set->count;
    out_snapshot->revision = set->revision;
    out_snapshot->exclude_hidden = set->exclude_hidden;
    out_snapshot->exclude_generated = set->exclude_generated;
    out_snapshot->exclude_binary = set->exclude_binary;
    for (index = 0U; index < set->count; ++index) {
        const UmiEditorWorkspaceSearchExclusionRule *rule =
            &set->rules[index].rule;
        if (rule->enabled) ++out_snapshot->enabled_rule_count;
        if (rule->action == UMI_EDITOR_WORKSPACE_SEARCH_RULE_INCLUDE) {
            ++out_snapshot->include_rule_count;
        } else {
            ++out_snapshot->exclude_rule_count;
        }
    }
    return UMI_STATUS_OK;
}

size_t umi_editor_workspace_search_exclusion_rule_count(
    const UmiEditorWorkspaceSearchExclusionSet *set)
{
    return set != NULL ? set->count : 0U;
}

uint64_t umi_editor_workspace_search_exclusion_revision(
    const UmiEditorWorkspaceSearchExclusionSet *set)
{
    return set != NULL ? set->revision : 0U;
}
