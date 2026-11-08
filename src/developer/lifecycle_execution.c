/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/lifecycle_execution.c
 *
 * PURPOSE:
 *   Validate the full lifecycle request before invoking existing host services in canonical plan order.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/lifecycle_execution.h"
#include <limits.h>
#include <string.h>

static int lifecycle_value(const char *value)
{
    return value != NULL && value[0] != '\0' && value[0] != '-';
}
static int lifecycle_message(const char *value)
{
    if (!lifecycle_value(value)) return 0;
    for (; *value != '\0'; ++value)
        if (*value != ' ' && *value != '\t' && *value != '\r' && *value != '\n')
            return 1;
    return 0;
}
static int lifecycle_jobs(const char *value)
{
    unsigned int number = 0U;
    if (!lifecycle_value(value)) return 0;
    for (; *value != '\0'; ++value) {
        unsigned int digit;
        if (*value < '0' || *value > '9') return 0;
        digit = (unsigned int)(*value - '0');
        if (number > ((unsigned int)INT_MAX - digit) / 10U) return 0;
        number = number * 10U + digit;
    }
    return number != 0U;
}
static UmiStatus lifecycle_admit(const UmiDeveloperLifecycleExecutionRequest *r)
{
    const int builds = r->goal == UMI_DEVELOPER_LIFECYCLE_BUILD ||
        r->goal == UMI_DEVELOPER_LIFECYCLE_TEST || r->goal == UMI_DEVELOPER_LIFECYCLE_COMPLETE;
    const int publishes = r->goal == UMI_DEVELOPER_LIFECYCLE_PUBLISH ||
        r->goal == UMI_DEVELOPER_LIFECYCLE_COMPLETE;
    const int updates = r->goal == UMI_DEVELOPER_LIFECYCLE_UPDATE;
    if (r->goal < UMI_DEVELOPER_LIFECYCLE_VERIFY ||
        r->goal > UMI_DEVELOPER_LIFECYCLE_COMPLETE || !lifecycle_value(r->source) ||
        (r->strict != 0 && r->strict != 1) || (r->preview != 0 && r->preview != 1) ||
        (r->no_submodules != 0 && r->no_submodules != 1))
        return UMI_STATUS_INVALID_ARGUMENT;
    if ((r->preset != NULL && (!builds || !lifecycle_value(r->preset))) ||
        (r->jobs != NULL && (!builds || !lifecycle_jobs(r->jobs))) ||
        (r->message != NULL && (!publishes || !lifecycle_message(r->message))) ||
        (r->remote != NULL && ((!publishes && !updates) || !lifecycle_value(r->remote))) ||
        (r->branch != NULL && ((!publishes && !updates) || !lifecycle_value(r->branch))) ||
        (r->strict && updates) || (r->no_submodules && !updates) ||
        (publishes && !r->preview && !lifecycle_message(r->message)))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
static UmiDeveloperLifecycleAction lifecycle_action(UmiDeveloperLifecycleStage stage)
{
    switch (stage) {
        case UMI_DEVELOPER_STAGE_ENVIRONMENT:
        case UMI_DEVELOPER_STAGE_DEPENDENCIES: return UMI_DEVELOPER_ACTION_ENVIRONMENT;
        case UMI_DEVELOPER_STAGE_SOURCE_QUALITY:
        case UMI_DEVELOPER_STAGE_SOURCE_SECURITY:
        case UMI_DEVELOPER_STAGE_ARCHITECTURE:
        case UMI_DEVELOPER_STAGE_MEMORY_STATIC: return UMI_DEVELOPER_ACTION_SOURCE_SCAN;
        case UMI_DEVELOPER_STAGE_DEPENDENCY_VULNERABILITIES: return UMI_DEVELOPER_ACTION_DEPENDENCY_AUDIT;
        case UMI_DEVELOPER_STAGE_CONFIGURE: return UMI_DEVELOPER_ACTION_CONFIGURE;
        case UMI_DEVELOPER_STAGE_COMPILE_LINK: return UMI_DEVELOPER_ACTION_BUILD;
        case UMI_DEVELOPER_STAGE_TEST: return UMI_DEVELOPER_ACTION_TEST;
        case UMI_DEVELOPER_STAGE_REPOSITORY_VERIFY: return UMI_DEVELOPER_ACTION_REPOSITORY_VERIFY;
        case UMI_DEVELOPER_STAGE_REPOSITORY_STAGE:
        case UMI_DEVELOPER_STAGE_REPOSITORY_COMMIT:
        case UMI_DEVELOPER_STAGE_REPOSITORY_PUSH: return UMI_DEVELOPER_ACTION_REPOSITORY_PUBLISH;
        case UMI_DEVELOPER_STAGE_REPOSITORY_UPDATE: return UMI_DEVELOPER_ACTION_REPOSITORY_UPDATE;
        default: return UMI_DEVELOPER_ACTION_NONE;
    }
}
void umi_developer_lifecycle_execution_request_init(
    UmiDeveloperLifecycleExecutionRequest *request, UmiDeveloperLifecycleGoal goal)
{
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->goal = goal;
    request->source = ".";
}
UmiStatus umi_developer_lifecycle_execute(
    const UmiDeveloperLifecycleExecutionRequest *request,
    const UmiDeveloperLifecycleExecutionHost *host,
    UmiDeveloperLifecycleExecutionReport *out_report)
{
    UmiDeveloperLifecycleExecutionRequest admitted;
    UmiDeveloperLifecyclePlanRequest plan_request;
    UmiDeveloperLifecycleExecutionHost admitted_host;
    size_t index;
    if (out_report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->status = UMI_STATUS_INVALID_ARGUMENT;
    if (request == NULL) return out_report->status;
    admitted = *request;
    out_report->status = lifecycle_admit(&admitted);
    if (out_report->status != UMI_STATUS_OK) return out_report->status;
    out_report->preview = admitted.preview;
    umi_developer_lifecycle_plan_request_init(&plan_request, admitted.goal);
    plan_request.require_external_vulnerability_scanner = admitted.strict;
    out_report->status = umi_developer_lifecycle_plan_build(&plan_request, &out_report->plan);
    if (out_report->status != UMI_STATUS_OK) return out_report->status;

    /* The existing plan owns order. Group adjacent stages only when their real
     * service is shared, so a scan or publication never executes several times. */
    for (index = 0U; index < out_report->plan.stage_count; ++index) {
        UmiDeveloperLifecycleAction action = lifecycle_action(out_report->plan.stages[index].stage);
        UmiDeveloperLifecycleActionResult *item;
        if (action == UMI_DEVELOPER_ACTION_NONE) {
            out_report->status = UMI_STATUS_NOT_IMPLEMENTED;
            return out_report->status;
        }
        if (out_report->action_count != 0U &&
            out_report->actions[out_report->action_count - 1U].action == action) {
            ++out_report->actions[out_report->action_count - 1U].stage_count;
            continue;
        }
        if (out_report->action_count >= UMI_DEVELOPER_ACTION_COUNT - 1U) {
            out_report->status = UMI_STATUS_CAPACITY_EXCEEDED;
            return out_report->status;
        }
        item = &out_report->actions[out_report->action_count++];
        item->action = action;
        item->first_stage = index;
        item->stage_count = 1U;
    }
    if (admitted.preview) return UMI_STATUS_OK;
    if (host == NULL) {
        out_report->status = UMI_STATUS_UNAVAILABLE;
        return out_report->status;
    }
    admitted_host = *host;
    /* Validate the entire host before an earlier action can mutate anything. */
    for (index = 0U; index < out_report->action_count; ++index) {
        if (admitted_host.execute[out_report->actions[index].action] == NULL) {
            out_report->status = UMI_STATUS_UNAVAILABLE;
            return out_report->status;
        }
    }
    for (index = 0U; index < out_report->action_count; ++index) {
        UmiDeveloperLifecycleActionResult *item = &out_report->actions[index];
        item->attempted = 1;
        ++out_report->attempted_count;
        item->backend_result = admitted_host.execute[item->action](
            admitted_host.user_data, &admitted, item->action);
        if (item->backend_result != 0) {
            out_report->failed_action = item->action;
            out_report->backend_result = item->backend_result;
            out_report->status = UMI_STATUS_IO_ERROR;
            return out_report->status;
        }
        item->completed = 1;
        ++out_report->completed_count;
    }
    return UMI_STATUS_OK;
}
const char *umi_developer_lifecycle_action_text(UmiDeveloperLifecycleAction action)
{
    switch (action) {
        case UMI_DEVELOPER_ACTION_ENVIRONMENT: return "environment and native dependencies";
        case UMI_DEVELOPER_ACTION_SOURCE_SCAN: return "source scan";
        case UMI_DEVELOPER_ACTION_DEPENDENCY_AUDIT: return "dependency audit";
        case UMI_DEVELOPER_ACTION_CONFIGURE: return "configure";
        case UMI_DEVELOPER_ACTION_BUILD: return "compile and link";
        case UMI_DEVELOPER_ACTION_TEST: return "tests";
        case UMI_DEVELOPER_ACTION_REPOSITORY_VERIFY: return "repository review";
        case UMI_DEVELOPER_ACTION_REPOSITORY_PUBLISH: return "repository publication";
        case UMI_DEVELOPER_ACTION_REPOSITORY_UPDATE: return "repository update";
        default: return "no action";
    }
}
