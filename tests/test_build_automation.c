/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_automation.c
 *
 * PURPOSE:
 *   Prove that the Automated Build System selects a changed application alone,
 *   expands shared Framework changes to consumers and keeps documentation-only
 *   edits out of compilation and test actions.
 *
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

#include "umicom/build/automation.h"

/* Register one scope with the exact targets that an execution adapter uses. */
static void register_scope(UmiBuildAutomation *automation,
                           const char *scope_id,
                           const char *prefix,
                           const char *build_target,
                           const char *test_target,
                           const char *test_expression,
                           UmiBuildAutomationScopeFlags flags)
{
    UmiBuildAutomationScope scope;

    umi_build_automation_scope_init(
        &scope, scope_id, scope_id, prefix);
    assert(snprintf(scope.build_target,
                    sizeof(scope.build_target),
                    "%s",
                    build_target) >= 0);
    assert(snprintf(scope.test_build_target,
                    sizeof(scope.test_build_target),
                    "%s",
                    test_target) >= 0);
    assert(snprintf(scope.test_expression,
                    sizeof(scope.test_expression),
                    "%s",
                    test_expression) >= 0);
    scope.flags = flags;
    assert(umi_build_automation_register_scope(automation, &scope) ==
           UMI_STATUS_OK);
}

/* Add one changed path through the same public classification path used by clients. */
static void add_change(UmiBuildAutomation *automation, const char *path)
{
    UmiBuildAutomationChange change;

    umi_build_automation_change_init(&change, path, 0);
    assert(umi_build_automation_add_change(automation, &change) ==
           UMI_STATUS_OK);
}

/* Build the small representative workspace used by each independent scenario. */
static UmiBuildAutomation *create_workspace(void)
{
    UmiBuildAutomation *automation = NULL;

    assert(umi_build_automation_create(&automation) == UMI_STATUS_OK);
    register_scope(automation,
                   "workspace",
                   "",
                   "umicom-products",
                   "umicom-tests",
                   "",
                   UMI_BUILD_AUTOMATION_SCOPE_WORKSPACE_WIDE);
    register_scope(automation,
                   "framework",
                   "framework",
                   "umicom-products",
                   "umicom-tests-framework",
                   "^framework\\.",
                   UMI_BUILD_AUTOMATION_SCOPE_SHARED_PROVIDER);
    register_scope(automation,
                   "studio",
                   "applications/studio",
                   "umicom-product-studio",
                   "umicom-tests-studio",
                   "^studio\\.",
                   UMI_BUILD_AUTOMATION_SCOPE_CONSUMES_SHARED);
    register_scope(automation,
                   "trader",
                   "applications/trader",
                   "umicom-product-trader",
                   "umicom-tests-trader",
                   "^trader\\.",
                   UMI_BUILD_AUTOMATION_SCOPE_CONSUMES_SHARED);
    return automation;
}

/* Verify that one application edit does not select unrelated applications. */
static void test_application_selection(void)
{
    UmiBuildAutomation *automation = create_workspace();
    UmiBuildAutomationChange change;
    UmiBuildAutomationPlanItem item;
    UmiBuildAutomationSnapshot snapshot;

    add_change(automation, ".\\applications\\studio\\src\\app.c");
    assert(umi_build_automation_change_at(automation, 0U, &change) ==
           UMI_STATUS_OK);
    assert(strcmp(change.path, "applications/studio/src/app.c") == 0);
    assert(umi_build_automation_evaluate(automation) == UMI_STATUS_OK);
    assert(umi_build_automation_count(automation) == 1U);
    assert(umi_build_automation_item_at(automation, 0U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.scope_id, "studio") == 0);
    assert(strcmp(item.build_target, "umicom-product-studio") == 0);
    assert(item.direct_change != 0);
    assert((item.actions & UMI_BUILD_AUTOMATION_ACTION_BUILD) != 0U);
    assert((item.actions & UMI_BUILD_AUTOMATION_ACTION_TEST) != 0U);
    assert(umi_build_automation_snapshot(automation, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.direct_count == 1U);
    assert(snapshot.inherited_count == 0U);
    umi_build_automation_destroy(automation);
}

/* Verify that a public Framework contract selects every registered consumer. */
static void test_shared_contract_selection(void)
{
    UmiBuildAutomation *automation = create_workspace();
    UmiBuildAutomationPlanItem item;
    UmiBuildAutomationSnapshot snapshot;

    add_change(automation, "framework/include/umicom/build/automation.h");
    assert(umi_build_automation_evaluate(automation) == UMI_STATUS_OK);
    assert(umi_build_automation_count(automation) == 3U);
    assert(umi_build_automation_item_at(automation, 0U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.scope_id, "framework") == 0);
    assert(item.direct_change != 0);
    assert(umi_build_automation_item_at(automation, 1U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.scope_id, "studio") == 0);
    assert(item.inherited_change != 0);
    assert(umi_build_automation_item_at(automation, 2U, &item) ==
           UMI_STATUS_OK);
    assert(strcmp(item.scope_id, "trader") == 0);
    assert(umi_build_automation_snapshot(automation, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.direct_count == 1U);
    assert(snapshot.inherited_count == 2U);
    umi_build_automation_destroy(automation);
}

/* Verify that documentation changes update documentation without compiling. */
static void test_documentation_selection(void)
{
    UmiBuildAutomation *automation = create_workspace();
    UmiBuildAutomationPlanItem item;

    add_change(automation, "framework/docs/automated-build-system.md");
    assert(umi_build_automation_evaluate(automation) == UMI_STATUS_OK);
    assert(umi_build_automation_count(automation) == 1U);
    assert(umi_build_automation_item_at(automation, 0U, &item) ==
           UMI_STATUS_OK);
    assert(item.actions == UMI_BUILD_AUTOMATION_ACTION_DOCUMENT);
    assert(item.inherited_change == 0);
    umi_build_automation_destroy(automation);
}

/* Run the independent scenarios and return success only when every assertion holds. */
int main(void)
{
    char actions[128];

    assert(umi_build_automation_classify_path("src/kernel.asm") ==
           UMI_BUILD_AUTOMATION_CHANGE_ASSEMBLY);
    assert(umi_build_automation_classify_path("CMakeLists.txt") ==
           UMI_BUILD_AUTOMATION_CHANGE_BUILD_DEFINITION);
    assert(strcmp(umi_build_automation_action_text(
                      UMI_BUILD_AUTOMATION_ACTION_BUILD |
                          UMI_BUILD_AUTOMATION_ACTION_TEST,
                      actions,
                      sizeof(actions)),
                  "build,test") == 0);
    test_application_selection();
    test_shared_contract_selection();
    test_documentation_selection();
    return 0;
}
