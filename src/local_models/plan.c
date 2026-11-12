/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/local_models/plan.c
 *
 * PURPOSE:
 *   Build bounded shell-free CPU server plans without launching or inspecting a model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "umicom/platform/output_file.h"
#include <stdio.h>
#include <string.h>

UmiStatus UmiModelRunPlanCreate(const char *executable, const char *model,
    const char *sha256, const char *alias, uint16_t port, uint32_t context,
    uint32_t threads, UmiModelRunPlan *outPlan)
{
    if (outPlan == NULL || !UmiModelTextFits(executable, UMI_MODEL_PATH_CAPACITY) ||
        !UmiModelTextFits(model, UMI_MODEL_PATH_CAPACITY) ||
        UmiOutputFileValidatePath(executable) != UMI_STATUS_OK ||
        UmiOutputFileValidatePath(model) != UMI_STATUS_OK ||
        !UmiModelDigestValid(sha256, 64U) || !UmiModelNameValid(alias, 128U, false) ||
        alias[0] == '-' || port < 1024U || context < 512U || context > 32768U ||
        threads == 0U || threads > 256U) return UMI_STATUS_INVALID_ARGUMENT;
    UmiModelRunPlan plan = {0};
    memcpy(plan.executable, executable, strlen(executable) + 1U);
    memcpy(plan.model, model, strlen(model) + 1U);
    memcpy(plan.sha256, sha256, 65U);
    memcpy(plan.alias, alias, strlen(alias) + 1U);
    memcpy(plan.directory, model, strlen(model) + 1U);
    /* Capture the model's parent explicitly; process behaviour must not depend
     * on whichever project happens to be the application's current directory. */
    char *separator = strrchr(plan.directory, '/');
    char *backslash = strrchr(plan.directory, '\\');
    if (backslash != NULL && (separator == NULL || backslash > separator)) separator = backslash;
    if (separator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (separator == plan.directory || (separator == plan.directory + 2 && plan.directory[1] == ':'))
        separator[1] = '\0';
    else *separator = '\0';
    int count = snprintf(plan.guard, sizeof(plan.guard), "%s.umicom-run.lock", model);
    if (count < 0 || (size_t)count >= sizeof(plan.guard)) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)snprintf(plan.port, sizeof(plan.port), "%u", (unsigned)port);
    (void)snprintf(plan.context, sizeof(plan.context), "%u", (unsigned)context);
    (void)snprintf(plan.threads, sizeof(plan.threads), "%u", (unsigned)threads);
    *outPlan = plan;
    return UMI_STATUS_OK;
}
UmiStatus UmiModelRunPlanValidate(const UmiModelRunPlan *plan)
{
    if (plan == NULL || !UmiModelTextFits(plan->port, sizeof(plan->port)) ||
        !UmiModelTextFits(plan->context, sizeof(plan->context)) ||
        !UmiModelTextFits(plan->threads, sizeof(plan->threads)) ||
        !UmiModelTextFits(plan->directory, sizeof(plan->directory)) ||
        !UmiModelTextFits(plan->guard, sizeof(plan->guard))) return UMI_STATUS_INVALID_ARGUMENT;
    uint64_t port, context, threads;
    if (!UmiModelNumber(plan->port, 1024U, 65535U, &port) ||
        !UmiModelNumber(plan->context, 512U, 32768U, &context) ||
        !UmiModelNumber(plan->threads, 1U, 256U, &threads)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiModelRunPlan checked;
    UmiStatus status = UmiModelRunPlanCreate(plan->executable, plan->model, plan->sha256, plan->alias,
        (uint16_t)port, (uint32_t)context, (uint32_t)threads, &checked);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(checked.guard, plan->guard) != 0 || strcmp(checked.directory, plan->directory) != 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus UmiModelRunArguments(const UmiModelRunPlan *plan,
    const char **arguments, size_t capacity, size_t *outCount)
{
    if (outCount != NULL) *outCount = 0U;
    if (arguments == NULL || outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiModelRunPlanValidate(plan);
    if (status != UMI_STATUS_OK) return status;
    if (capacity < UMI_MODEL_RUN_ARGUMENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    const char *values[] = {"--model", plan->model, "--host", "127.0.0.1",
        "--port", plan->port, "--ctx-size", plan->context, "--threads", plan->threads,
        "--n-gpu-layers", "0", "--parallel", "1", "--alias", plan->alias,
        "--offline", "--no-webui"};
    _Static_assert(sizeof(values) / sizeof(values[0]) == UMI_MODEL_RUN_ARGUMENTS, "argument capacity");
    memcpy(arguments, values, sizeof(values));
    *outCount = UMI_MODEL_RUN_ARGUMENTS;
    return UMI_STATUS_OK;
}
