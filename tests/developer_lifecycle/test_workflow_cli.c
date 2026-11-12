/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_lifecycle/test_workflow_cli.c
 *
 * PURPOSE:
 *   Verify real native workflow parsing and argument forwarding through controlled backend adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../tools/umicom/src/cli.h"
#include "umicom/developer/lifecycle_execution.h"
#include "../support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
/* Inclusion tests share a translation unit with the production parser.
 * Give captured backend calls a fixture-specific name so production-local
 * option tracking remains independent. Keep the former declaration for review. */
#if 0
static UmiDeveloperLifecycleAction seen[10];
#endif
static UmiDeveloperLifecycleAction captured_workflow_actions[10];
static size_t calls;
static UmiDeveloperLifecycleAction fail_action;
static int strict_audit, full_options, update_options;
static const char *source = "selected project";
static int Record(UmiDeveloperLifecycleAction action)
{
    assert(calls < 10U); captured_workflow_actions[calls++] = action;
    return action == fail_action ? 47 : 0;
}
static void Equal(const char *actual, const char *expected)
{
    assert(actual != NULL && !strcmp(actual, expected));
}
int umi_cli_command_check(UmiCliContext *context, int argc, char **argv)
{
    assert(context != NULL && argc == 3); Equal(argv[0], "--all");
    Equal(argv[1], "--project"); Equal(argv[2], source);
    return Record(UMI_DEVELOPER_ACTION_ENVIRONMENT);
}
int umi_cli_command_quality(UmiCliContext *context, int argc, char **argv)
{
    assert(context != NULL && argc == 5); Equal(argv[0], "scan"); Equal(argv[1], source);
    Equal(argv[2], "--profile"); Equal(argv[3], "ci"); Equal(argv[4], "--summary");
    return Record(UMI_DEVELOPER_ACTION_SOURCE_SCAN);
}
int umi_cli_command_dependencies(UmiCliContext *context, int argc, char **argv)
{
    assert(context != NULL && argc == (strict_audit ? 3 : 2));
    Equal(argv[0], "audit"); Equal(argv[1], source);
    if (strict_audit) Equal(argv[2], "--strict");
    return Record(UMI_DEVELOPER_ACTION_DEPENDENCY_AUDIT);
}
int umi_cli_command_build(UmiCliContext *context, UmiBuildAction action, int argc, char **argv)
{
    assert(context != NULL); Equal(argv[0], "--source"); Equal(argv[1], source);
    assert(argc == (full_options ? (action == UMI_BUILD_COMPILE ? 6 : 4) : 2));
    if (full_options) { Equal(argv[2], "--preset"); Equal(argv[3], "debug"); }
    if (full_options && action == UMI_BUILD_COMPILE) { Equal(argv[4], "--jobs"); Equal(argv[5], "2"); }
    return Record(action == UMI_BUILD_CONFIGURE ? UMI_DEVELOPER_ACTION_CONFIGURE :
        action == UMI_BUILD_COMPILE ? UMI_DEVELOPER_ACTION_BUILD : UMI_DEVELOPER_ACTION_TEST);
}
int umi_cli_command_repo(UmiCliContext *context, int argc, char **argv)
{
    assert(context != NULL); Equal(argv[1], source);
    if (!strcmp(argv[0], "verify")) { assert(argc == 2); return Record(UMI_DEVELOPER_ACTION_REPOSITORY_VERIFY); }
    if (!strcmp(argv[0], "publish")) {
        assert(argc == (full_options ? 8 : 4)); Equal(argv[2], "--message"); Equal(argv[3], "Reviewed change");
        if (full_options) { Equal(argv[4], "--remote"); Equal(argv[5], "origin"); Equal(argv[6], "--branch"); Equal(argv[7], "main"); }
        return Record(UMI_DEVELOPER_ACTION_REPOSITORY_PUBLISH);
    }
    Equal(argv[0], "update"); assert(argc == (update_options ? 7 : 2));
    if (update_options) { Equal(argv[2], "--remote"); Equal(argv[3], "origin"); Equal(argv[4], "--branch"); Equal(argv[5], "main"); Equal(argv[6], "--no-submodules"); }
    return Record(UMI_DEVELOPER_ACTION_REPOSITORY_UPDATE);
}
#include "../../tools/umicom/src/command_workflow.c"
int main(int argc, char **argv)
{
    UmiCliContext context = {0};
    char *args[32] = {"complete", "--source", "selected project", "--message", "Reviewed change"};
    int length = 5, expected = 2;
    size_t expected_calls = 0U;
    const char *mode;
    if (argc != 2) return 2;
    mode = argv[1];
    if (!strcmp(mode, "complete")) { expected = 0; expected_calls = 8U; }
    else if (!strcmp(mode, "full-options")) {
        const char *extra[] = {"--preset", "debug", "--jobs", "2", "--remote", "origin", "--branch", "main", "--strict"};
        for (size_t i = 0U; i < 9U; ++i) args[length++] = (char *)extra[i];
        full_options = strict_audit = 1; expected = 0; expected_calls = 8U;
    } else if (!strcmp(mode, "verify") || !strcmp(mode, "build") || !strcmp(mode, "test")) {
        args[0] = (char *)mode; length = 3; expected = 0;
        expected_calls = !strcmp(mode, "verify") ? 3U : !strcmp(mode, "build") ? 5U : 6U;
    } else if (!strcmp(mode, "publish")) { args[0] = "publish"; expected = 0; expected_calls = 5U; }
    else if (!strcmp(mode, "update")) { args[0] = "update"; length = 3; expected = 0; expected_calls = 2U; }
    else if (!strcmp(mode, "update-options")) {
        args[0] = "update"; length = 3;
        const char *extra[] = {"--remote", "origin", "--branch", "main", "--no-submodules"};
        for (size_t i = 0U; i < 5U; ++i) args[length++] = (char *)extra[i];
        update_options = 1; expected = 0; expected_calls = 2U;
    } else if (!strcmp(mode, "plan")) { args[0] = "plan"; args[3] = "--goal"; args[4] = "complete"; expected = 0; }
    else if (!strcmp(mode, "dry-run")) { args[3] = "--dry-run"; length = 4; expected = 0; }
    else if (!strcmp(mode, "help")) { args[0] = "--help"; length = 1; expected = 0; }
    else if (!strcmp(mode, "empty")) { length = 0; expected = 0; }
    else if (!strncmp(mode, "fail-", 5U)) {
        fail_action = (UmiDeveloperLifecycleAction)atoi(mode + 5); expected = 47;
        assert(fail_action >= 1 && fail_action <= 9); expected_calls = (size_t)fail_action;
        if (fail_action == UMI_DEVELOPER_ACTION_REPOSITORY_UPDATE) { args[0] = "update"; length = 3; expected_calls = 2U; }
    } else if (!strncmp(mode, "duplicate-", 10U)) {
        static const char *names[] = {"--source", "--preset", "--jobs", "--message", "--remote", "--branch", "--goal", "--strict", "--dry-run", "--no-submodules"};
        int index = atoi(mode + 10); assert(index >= 0 && index < 10);
        args[length++] = (char *)names[index]; if (index < 7) args[length++] = "2";
        args[length++] = (char *)names[index]; if (index < 7) args[length++] = "3";
    } else if (!strcmp(mode, "negative")) length = -1;
    else if (!strcmp(mode, "null-vector")) { assert(umi_cli_command_workflow(&context, 1, NULL) == 2 && calls == 0U); return 0; }
    else if (!strcmp(mode, "null-command")) args[0] = NULL;
    else if (!strcmp(mode, "null-option")) args[3] = NULL;
    else if (!strcmp(mode, "null-value")) args[4] = NULL;
    else if (!strcmp(mode, "empty-value")) args[4] = "";
    else if (!strcmp(mode, "missing-value")) length = 4;
    else if (!strcmp(mode, "unknown-option")) args[3] = "--force";
    else if (!strcmp(mode, "unknown-command")) args[0] = "everything";
    else if (!strcmp(mode, "goal-on-complete")) { args[length++] = "--goal"; args[length++] = "build"; }
    else if (!strcmp(mode, "goal-plan")) { args[0] = "plan"; args[3] = "--goal"; args[4] = "plan"; }
    else if (!strcmp(mode, "goal-unknown")) { args[0] = "plan"; args[3] = "--goal"; args[4] = "other"; }
    else if (!strcmp(mode, "missing-message")) length = 3;
    else if (!strcmp(mode, "blank-message")) args[4] = " \t\r\n";
    else if (!strcmp(mode, "jobs-zero")) { args[length++] = "--jobs"; args[length++] = "0"; }
    else if (!strcmp(mode, "jobs-overflow")) { args[length++] = "--jobs"; args[length++] = "999999999999999"; }
    else if (!strcmp(mode, "preset-on-publish")) { args[0] = "publish"; args[length++] = "--preset"; args[length++] = "debug"; }
    else if (!strcmp(mode, "submodules-on-complete")) args[length++] = "--no-submodules";
    else if (!strcmp(mode, "strict-on-update")) { args[0] = "update"; args[3] = "--strict"; length = 4; }
    else if (!strcmp(mode, "null-context")) { assert(umi_cli_command_workflow(NULL, length, args) == 2 && calls == 0U); return 0; }
    else return 2;
    assert(umi_cli_command_workflow(&context, length, args) == expected);
    assert(calls == expected_calls);
    if (calls != 0U) {
        if (!strcmp(args[0], "update")) {
            assert(captured_workflow_actions[0] == UMI_DEVELOPER_ACTION_REPOSITORY_VERIFY);
            if (calls > 1U) assert(captured_workflow_actions[1] == UMI_DEVELOPER_ACTION_REPOSITORY_UPDATE);
        } else {
            assert(captured_workflow_actions[0] == UMI_DEVELOPER_ACTION_ENVIRONMENT);
            if (calls >= 3U) assert(captured_workflow_actions[2] == UMI_DEVELOPER_ACTION_DEPENDENCY_AUDIT);
            if (!strcmp(args[0], "complete")) for (size_t i = 0U; i < calls; ++i) assert(captured_workflow_actions[i] == (UmiDeveloperLifecycleAction)(i + 1U));
            if (!strcmp(args[0], "publish") && calls == 5U) assert(captured_workflow_actions[3] == UMI_DEVELOPER_ACTION_REPOSITORY_VERIFY && captured_workflow_actions[4] == UMI_DEVELOPER_ACTION_REPOSITORY_PUBLISH);
        }
    }
    return 0;
}
