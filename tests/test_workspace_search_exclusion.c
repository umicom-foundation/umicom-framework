/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_exclusion.c
 *
 * PURPOSE:
 *   Implement the test workspace search exclusion behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_workspace_search_exclusion.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/editor/workspace_search_exclusion.h"

/*
 * Exercise facts for and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiEditorWorkspaceSearchResourceFacts facts_for(const char *path,
                                                        int generated)
{
    UmiEditorWorkspaceSearchResourceFacts facts;
    (void)memset(&facts, 0, sizeof(facts));
    facts.struct_size = (uint32_t)sizeof(facts);
    facts.api_version = UMI_EDITOR_WORKSPACE_SEARCH_EXCLUSION_API_VERSION;
    facts.uri = path;
    facts.relative_path = path;
    facts.file_name = strrchr(path, '/') != NULL
        ? strrchr(path, '/') + 1 : path;
    facts.extension = "c";
    facts.language_id = "c";
    facts.generated = generated;
    return facts;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEditorWorkspaceSearchExclusionSet *set = NULL;
    UmiEditorWorkspaceSearchExclusionRule rule;
    UmiEditorWorkspaceSearchExclusionDecision decision;
    UmiEditorWorkspaceSearchResourceFacts facts;

    assert(umi_editor_workspace_search_exclusion_set_create(&set) ==
           UMI_STATUS_OK);
    facts = facts_for("build/generated.c", 1);
    assert(umi_editor_workspace_search_exclusion_evaluate(
               set, &facts, &decision) == UMI_STATUS_OK);
    assert(!decision.included);
    assert(decision.reason == UMI_EDITOR_WORKSPACE_SEARCH_EXCLUDED_GENERATED);

    (void)memset(&rule, 0, sizeof(rule));
    rule.struct_size = (uint32_t)sizeof(rule);
    rule.api_version = UMI_EDITOR_WORKSPACE_SEARCH_EXCLUSION_API_VERSION;
    (void)snprintf(rule.id, sizeof(rule.id), "%s", "exclude-build");
    (void)snprintf(rule.pattern, sizeof(rule.pattern), "%s", "build/**");
    (void)snprintf(rule.reason, sizeof(rule.reason), "%s",
                   "Build output is excluded");
    rule.action = UMI_EDITOR_WORKSPACE_SEARCH_RULE_EXCLUDE;
    rule.target = UMI_EDITOR_WORKSPACE_SEARCH_RULE_PATH;
    rule.syntax = UMI_EDITOR_WORKSPACE_SEARCH_RULE_GLOB;
    rule.priority = 100;
    rule.enabled = 1;
    assert(umi_editor_workspace_search_exclusion_set_upsert(set, &rule) ==
           UMI_STATUS_OK);

    facts = facts_for("src/main.c", 0);
    assert(umi_editor_workspace_search_exclusion_evaluate(
               set, &facts, &decision) == UMI_STATUS_OK);
    assert(decision.included);

    facts = facts_for("build/main.c", 0);
    assert(umi_editor_workspace_search_exclusion_evaluate(
               set, &facts, &decision) == UMI_STATUS_OK);
    assert(!decision.included);
    assert(strcmp(decision.matching_rule_id, "exclude-build") == 0);

    (void)snprintf(rule.id, sizeof(rule.id), "%s", "include-keep");
    (void)snprintf(rule.pattern, sizeof(rule.pattern), "%s",
                   "build/keep.c");
    (void)snprintf(rule.reason, sizeof(rule.reason), "%s",
                   "Explicitly retained generated source");
    rule.action = UMI_EDITOR_WORKSPACE_SEARCH_RULE_INCLUDE;
    rule.syntax = UMI_EDITOR_WORKSPACE_SEARCH_RULE_LITERAL;
    rule.priority = 200;
    assert(umi_editor_workspace_search_exclusion_set_upsert(set, &rule) ==
           UMI_STATUS_OK);
    facts = facts_for("build/keep.c", 1);
    assert(umi_editor_workspace_search_exclusion_evaluate(
               set, &facts, &decision) == UMI_STATUS_OK);
    assert(decision.included);
    assert(strcmp(decision.matching_rule_id, "include-keep") == 0);

    umi_editor_workspace_search_exclusion_set_destroy(set);
    return 0;
}
