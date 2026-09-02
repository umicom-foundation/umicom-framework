/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_project/run_plan.c
 *
 * PURPOSE:
 *   Implement shell-independent run planning across native and script projects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_project/run_plan.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise developer project run plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_project_run_plan_init(
    UmiDeveloperProjectRunPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->revision = 1U;
}

/*
 * Provide the developer project run plan native operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_run_plan_native(
    UmiDeveloperProjectRunPlan *plan,
    const char *language_id,
    const char *executable,
    const char *working_directory)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || language_id == NULL ||
        executable == NULL || executable[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_project_run_plan_init(plan);
    status = copy_text(
        plan->language_id, sizeof(plan->language_id), language_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_build_command_init(&plan->command, executable);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (working_directory != NULL && working_directory[0] != '\0' &&
        !umi_build_command_set_working_directory(
            &plan->command, working_directory)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    plan->debugger_eligible = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer project run plan interpreter operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_run_plan_interpreter(
    UmiDeveloperProjectRunPlan *plan,
    const char *language_id,
    const char *interpreter,
    const char *entry_file,
    const char *working_directory)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || language_id == NULL ||
        interpreter == NULL || interpreter[0] == '\0' ||
        entry_file == NULL || entry_file[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_project_run_plan_init(plan);
    status = copy_text(
        plan->language_id, sizeof(plan->language_id), language_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_build_command_init(&plan->command, interpreter);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_build_command_add_argument(&plan->command, entry_file)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (working_directory != NULL && working_directory[0] != '\0' &&
        !umi_build_command_set_working_directory(
            &plan->command, working_directory)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the developer project run plan cargo operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_run_plan_cargo(
    UmiDeveloperProjectRunPlan *plan,
    const char *working_directory,
    int release)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || working_directory == NULL ||
        working_directory[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_project_run_plan_init(plan);
    status = copy_text(
        plan->language_id,
        sizeof(plan->language_id),
        "developer.language.rust");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_build_command_init(&plan->command, "cargo");
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_build_command_add_argument(&plan->command, "run")) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (release &&
        !umi_build_command_add_argument(&plan->command, "--release")) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_build_command_set_working_directory(
            &plan->command, working_directory)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    plan->debugger_eligible = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer project run plan zig operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_project_run_plan_zig(
    UmiDeveloperProjectRunPlan *plan,
    const char *working_directory)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || working_directory == NULL ||
        working_directory[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_developer_project_run_plan_init(plan);
    status = copy_text(
        plan->language_id,
        sizeof(plan->language_id),
        "developer.language.zig");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_build_command_init(&plan->command, "zig");
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_build_command_add_argument(&plan->command, "build") ||
        !umi_build_command_add_argument(&plan->command, "run") ||
        !umi_build_command_set_working_directory(
            &plan->command, working_directory)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    plan->debugger_eligible = 1;
    return UMI_STATUS_OK;
}
