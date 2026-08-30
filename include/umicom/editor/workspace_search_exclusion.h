/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workspace_search_exclusion.h
 *
 * PURPOSE:
 *   Define deterministic include/exclude rules for reusable workspace search.
 *   Rules are independent of .gitignore parsing and can be populated from
 *   product settings, workspace configuration, generated-file catalogues,
 *   security policy or an external source-control provider.
 *
 * OWNERSHIP:
 *   Rule sets copy every rule. Evaluation returns a value snapshot and never
 *   exposes internal storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKSPACE_SEARCH_EXCLUSION_H
#define UMICOM_EDITOR_WORKSPACE_SEARCH_EXCLUSION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_WORKSPACE_SEARCH_EXCLUSION_API_VERSION 1U
#define UMI_EDITOR_WORKSPACE_SEARCH_RULE_ID_CAPACITY 128U
#define UMI_EDITOR_WORKSPACE_SEARCH_RULE_PATTERN_CAPACITY 512U
#define UMI_EDITOR_WORKSPACE_SEARCH_RULE_REASON_CAPACITY 256U
#define UMI_EDITOR_WORKSPACE_SEARCH_LANGUAGE_ID_CAPACITY 64U

typedef enum UmiEditorWorkspaceSearchRuleAction {
    UMI_EDITOR_WORKSPACE_SEARCH_RULE_INCLUDE = 1,
    UMI_EDITOR_WORKSPACE_SEARCH_RULE_EXCLUDE = 2
} UmiEditorWorkspaceSearchRuleAction;

typedef enum UmiEditorWorkspaceSearchRuleTarget {
    UMI_EDITOR_WORKSPACE_SEARCH_RULE_PATH = 1,
    UMI_EDITOR_WORKSPACE_SEARCH_RULE_FILE_NAME = 2,
    UMI_EDITOR_WORKSPACE_SEARCH_RULE_EXTENSION = 3,
    UMI_EDITOR_WORKSPACE_SEARCH_RULE_LANGUAGE = 4
} UmiEditorWorkspaceSearchRuleTarget;

typedef enum UmiEditorWorkspaceSearchRuleSyntax {
    UMI_EDITOR_WORKSPACE_SEARCH_RULE_LITERAL = 1,
    UMI_EDITOR_WORKSPACE_SEARCH_RULE_GLOB = 2
} UmiEditorWorkspaceSearchRuleSyntax;

typedef struct UmiEditorWorkspaceSearchExclusionRule {
    uint32_t struct_size;
    uint32_t api_version;
    char id[UMI_EDITOR_WORKSPACE_SEARCH_RULE_ID_CAPACITY];
    char pattern[UMI_EDITOR_WORKSPACE_SEARCH_RULE_PATTERN_CAPACITY];
    char reason[UMI_EDITOR_WORKSPACE_SEARCH_RULE_REASON_CAPACITY];
    UmiEditorWorkspaceSearchRuleAction action;
    UmiEditorWorkspaceSearchRuleTarget target;
    UmiEditorWorkspaceSearchRuleSyntax syntax;
    int32_t priority;
    int case_sensitive;
    int enabled;
} UmiEditorWorkspaceSearchExclusionRule;

typedef struct UmiEditorWorkspaceSearchResourceFacts {
    uint32_t struct_size;
    uint32_t api_version;
    const char *uri;
    const char *relative_path;
    const char *file_name;
    const char *extension;
    const char *language_id;
    int hidden;
    int generated;
    int binary;
    int read_only;
} UmiEditorWorkspaceSearchResourceFacts;

typedef enum UmiEditorWorkspaceSearchExclusionReason {
    UMI_EDITOR_WORKSPACE_SEARCH_INCLUDED = 1,
    UMI_EDITOR_WORKSPACE_SEARCH_EXCLUDED_BY_RULE = 2,
    UMI_EDITOR_WORKSPACE_SEARCH_EXCLUDED_HIDDEN = 3,
    UMI_EDITOR_WORKSPACE_SEARCH_EXCLUDED_GENERATED = 4,
    UMI_EDITOR_WORKSPACE_SEARCH_EXCLUDED_BINARY = 5
} UmiEditorWorkspaceSearchExclusionReason;

typedef struct UmiEditorWorkspaceSearchExclusionDecision {
    uint32_t struct_size;
    uint32_t api_version;
    int included;
    UmiEditorWorkspaceSearchExclusionReason reason;
    char matching_rule_id[UMI_EDITOR_WORKSPACE_SEARCH_RULE_ID_CAPACITY];
    char explanation[UMI_EDITOR_WORKSPACE_SEARCH_RULE_REASON_CAPACITY];
    int32_t matching_priority;
    uint64_t rule_set_revision;
} UmiEditorWorkspaceSearchExclusionDecision;

typedef struct UmiEditorWorkspaceSearchExclusionSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t rule_count;
    size_t enabled_rule_count;
    size_t include_rule_count;
    size_t exclude_rule_count;
    uint64_t revision;
    int exclude_hidden;
    int exclude_generated;
    int exclude_binary;
} UmiEditorWorkspaceSearchExclusionSnapshot;

typedef struct UmiEditorWorkspaceSearchExclusionSet
    UmiEditorWorkspaceSearchExclusionSet;

UmiStatus umi_editor_workspace_search_exclusion_set_create(
    UmiEditorWorkspaceSearchExclusionSet **out_set);
void umi_editor_workspace_search_exclusion_set_destroy(
    UmiEditorWorkspaceSearchExclusionSet *set);

UmiStatus umi_editor_workspace_search_exclusion_set_clear(
    UmiEditorWorkspaceSearchExclusionSet *set);

UmiStatus umi_editor_workspace_search_exclusion_set_policy(
    UmiEditorWorkspaceSearchExclusionSet *set,
    int exclude_hidden,
    int exclude_generated,
    int exclude_binary);

UmiStatus umi_editor_workspace_search_exclusion_set_upsert(
    UmiEditorWorkspaceSearchExclusionSet *set,
    const UmiEditorWorkspaceSearchExclusionRule *rule);

UmiStatus umi_editor_workspace_search_exclusion_set_remove(
    UmiEditorWorkspaceSearchExclusionSet *set,
    const char *rule_id);

UmiStatus umi_editor_workspace_search_exclusion_set_at(
    const UmiEditorWorkspaceSearchExclusionSet *set,
    size_t index,
    UmiEditorWorkspaceSearchExclusionRule *out_rule);

UmiStatus umi_editor_workspace_search_exclusion_evaluate(
    const UmiEditorWorkspaceSearchExclusionSet *set,
    const UmiEditorWorkspaceSearchResourceFacts *facts,
    UmiEditorWorkspaceSearchExclusionDecision *out_decision);

UmiStatus umi_editor_workspace_search_exclusion_snapshot(
    const UmiEditorWorkspaceSearchExclusionSet *set,
    UmiEditorWorkspaceSearchExclusionSnapshot *out_snapshot);

size_t umi_editor_workspace_search_exclusion_rule_count(
    const UmiEditorWorkspaceSearchExclusionSet *set);
uint64_t umi_editor_workspace_search_exclusion_revision(
    const UmiEditorWorkspaceSearchExclusionSet *set);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_WORKSPACE_SEARCH_EXCLUSION_H */
