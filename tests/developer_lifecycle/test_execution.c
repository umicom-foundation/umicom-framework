/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_lifecycle/test_execution.c
 *
 * PURPOSE:
 *   Verify lifecycle admission, canonical grouped order and exact fail-fast reports without invoking external tools.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/lifecycle_execution.h"
#include "../support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct Probe {
    UmiDeveloperLifecycleAction seen[UMI_DEVELOPER_ACTION_COUNT];
    size_t count;
    UmiDeveloperLifecycleAction fail;
    int result;
} Probe;
static int Execute(void *data, const UmiDeveloperLifecycleExecutionRequest *request,
    UmiDeveloperLifecycleAction action)
{
    Probe *probe = data;
    assert(request != NULL && !strcmp(request->source, "selected project"));
    assert(probe->count < UMI_DEVELOPER_ACTION_COUNT);
    probe->seen[probe->count++] = action;
    return action == probe->fail ? probe->result : 0;
}
int main(int argc, char **argv)
{
    UmiDeveloperLifecycleExecutionRequest request;
    UmiDeveloperLifecycleExecutionHost host = {0};
    UmiDeveloperLifecycleExecutionReport report;
    Probe probe = {0};
    UmiStatus expected = UMI_STATUS_OK, status;
    size_t expected_actions = 8U;
    const char *mode;
    if (argc != 2) return 2;
    mode = argv[1];
    umi_developer_lifecycle_execution_request_init(&request, UMI_DEVELOPER_LIFECYCLE_COMPLETE);
    request.source = "selected project";
    request.message = "Explain the actual change";
    host.user_data = &probe;
    for (size_t i = 1U; i < UMI_DEVELOPER_ACTION_COUNT; ++i) host.execute[i] = Execute;
    if (!strcmp(mode, "complete")) { request.preset = "debug"; request.jobs = "2"; }
    else if (!strcmp(mode, "verify")) { request.goal = UMI_DEVELOPER_LIFECYCLE_VERIFY; request.message = NULL; expected_actions = 3U; }
    else if (!strcmp(mode, "build")) { request.goal = UMI_DEVELOPER_LIFECYCLE_BUILD; request.message = NULL; expected_actions = 5U; }
    else if (!strcmp(mode, "test")) { request.goal = UMI_DEVELOPER_LIFECYCLE_TEST; request.message = NULL; expected_actions = 6U; }
    else if (!strcmp(mode, "publish")) { request.goal = UMI_DEVELOPER_LIFECYCLE_PUBLISH; expected_actions = 5U; }
    else if (!strcmp(mode, "update")) { request.goal = UMI_DEVELOPER_LIFECYCLE_UPDATE; request.message = NULL; request.no_submodules = 1; expected_actions = 2U; }
    else if (!strcmp(mode, "preview")) { request.preview = 1; request.message = NULL; }
    else if (!strcmp(mode, "preview-no-host")) {
        request.preview = 1; request.message = NULL;
        assert(umi_developer_lifecycle_execute(&request, NULL, &report) == UMI_STATUS_OK);
        assert(report.preview && report.action_count == 8U && report.attempted_count == 0U); return 0;
    } else if (!strcmp(mode, "strict")) request.strict = 1;
    else if (!strncmp(mode, "fail-", 5U)) {
        probe.fail = (UmiDeveloperLifecycleAction)atoi(mode + 5);
        probe.result = 47; expected = UMI_STATUS_IO_ERROR;
        assert(probe.fail >= 1 && probe.fail <= 9);
        if (probe.fail == UMI_DEVELOPER_ACTION_REPOSITORY_UPDATE) {
            request.goal = UMI_DEVELOPER_LIFECYCLE_UPDATE; request.message = NULL; expected_actions = 2U;
        }
    } else if (!strcmp(mode, "negative-backend")) {
        probe.fail = UMI_DEVELOPER_ACTION_BUILD; probe.result = -3; expected = UMI_STATUS_IO_ERROR;
    } else if (!strncmp(mode, "missing-", 8U)) {
        int index = atoi(mode + 8); assert(index >= 1 && index <= 9);
        host.execute[index] = NULL; expected = UMI_STATUS_UNAVAILABLE;
        if (index == 9) { request.goal = UMI_DEVELOPER_LIFECYCLE_UPDATE; request.message = NULL; }
    } else if (!strcmp(mode, "unused-host-slot")) host.execute[9] = NULL;
    else if (!strcmp(mode, "null-request")) {
        assert(umi_developer_lifecycle_execute(NULL, &host, &report) == UMI_STATUS_INVALID_ARGUMENT);
        assert(report.attempted_count == 0U && probe.count == 0U); return 0;
    } else if (!strcmp(mode, "null-report")) {
        assert(umi_developer_lifecycle_execute(&request, &host, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        assert(probe.count == 0U); return 0;
    } else if (!strcmp(mode, "null-host")) {
        assert(umi_developer_lifecycle_execute(&request, NULL, &report) == UMI_STATUS_UNAVAILABLE);
        assert(report.attempted_count == 0U && probe.count == 0U); return 0;
    } else {
        expected = UMI_STATUS_INVALID_ARGUMENT;
        if (!strcmp(mode, "missing-message")) request.message = NULL;
        else if (!strcmp(mode, "blank-message")) request.message = " \t\r\n";
        else if (!strcmp(mode, "empty-message")) request.message = "";
        else if (!strcmp(mode, "source-null")) request.source = NULL;
        else if (!strcmp(mode, "source-empty")) request.source = "";
        else if (!strcmp(mode, "source-option")) request.source = "--other";
        else if (!strcmp(mode, "goal-zero")) request.goal = 0;
        else if (!strcmp(mode, "goal-range")) request.goal = 99;
        else if (!strcmp(mode, "jobs-zero")) request.jobs = "0";
        else if (!strcmp(mode, "jobs-negative")) request.jobs = "-1";
        else if (!strcmp(mode, "jobs-plus")) request.jobs = "+1";
        else if (!strcmp(mode, "jobs-fraction")) request.jobs = "1.5";
        else if (!strcmp(mode, "jobs-overflow")) request.jobs = "99999999999999999999";
        else if (!strcmp(mode, "jobs-empty")) request.jobs = "";
        else if (!strcmp(mode, "preset-empty")) request.preset = "";
        else if (!strcmp(mode, "remote-option")) request.remote = "--force";
        else if (!strcmp(mode, "branch-empty")) request.branch = "";
        else if (!strcmp(mode, "strict-range")) request.strict = 2;
        else if (!strcmp(mode, "preview-range")) request.preview = -1;
        else if (!strcmp(mode, "submodules-range")) request.no_submodules = 2;
        else if (!strcmp(mode, "submodules-scope")) request.no_submodules = 1;
        else if (!strcmp(mode, "strict-update")) { request.goal = UMI_DEVELOPER_LIFECYCLE_UPDATE; request.message = NULL; request.strict = 1; }
        else if (!strcmp(mode, "preset-scope")) { request.goal = UMI_DEVELOPER_LIFECYCLE_PUBLISH; request.preset = "debug"; }
        else if (!strcmp(mode, "jobs-scope")) { request.goal = UMI_DEVELOPER_LIFECYCLE_PUBLISH; request.jobs = "2"; }
        else if (!strcmp(mode, "message-scope")) request.goal = UMI_DEVELOPER_LIFECYCLE_VERIFY;
        else if (!strcmp(mode, "remote-scope")) { request.goal = UMI_DEVELOPER_LIFECYCLE_BUILD; request.message = NULL; request.remote = "origin"; }
        else if (!strcmp(mode, "branch-scope")) { request.goal = UMI_DEVELOPER_LIFECYCLE_TEST; request.message = NULL; request.branch = "main"; }
        else return 2;
    }
    (void)memset(&report, 0xa5, sizeof(report));
    status = umi_developer_lifecycle_execute(&request, &host, &report);
    assert(status == expected && report.status == expected);
    if (expected == UMI_STATUS_INVALID_ARGUMENT || expected == UMI_STATUS_UNAVAILABLE) {
        assert(probe.count == 0U && report.attempted_count == 0U && report.completed_count == 0U);
        return 0;
    }
    assert(report.action_count == expected_actions);
    assert(report.plan.goal == request.goal);
    if (request.goal != UMI_DEVELOPER_LIFECYCLE_UPDATE) {
        assert(report.actions[0].first_stage == 0U && report.actions[0].stage_count == 2U);
        assert(report.actions[1].first_stage == 2U && report.actions[1].stage_count == 4U);
        assert(report.plan.stages[6].required == request.strict);
    }
    size_t covered = 0U;
    for (size_t i = 0U; i < report.action_count; ++i) {
        assert(report.actions[i].first_stage == covered);
        covered += report.actions[i].stage_count;
        if (i < probe.count) assert(probe.seen[i] == report.actions[i].action && report.actions[i].attempted);
        else assert(!report.actions[i].attempted && !report.actions[i].completed);
    }
    assert(covered == report.plan.stage_count && report.attempted_count == probe.count);
    if (request.preview) assert(probe.count == 0U && report.completed_count == 0U);
    else if (expected == UMI_STATUS_IO_ERROR) {
        assert(report.failed_action == probe.fail && report.backend_result == probe.result);
        assert(report.completed_count + 1U == report.attempted_count);
        assert(!report.actions[probe.count - 1U].completed);
        assert(report.actions[probe.count - 1U].backend_result == probe.result);
    } else assert(report.completed_count == expected_actions && report.failed_action == UMI_DEVELOPER_ACTION_NONE);
    if (request.goal == UMI_DEVELOPER_LIFECYCLE_COMPLETE || request.goal == UMI_DEVELOPER_LIFECYCLE_PUBLISH)
        assert(report.actions[report.action_count - 1U].stage_count == 3U);
    return 0;
}
