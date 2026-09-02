/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/run_plan.h
 *
 * PURPOSE:
 *   Plan safe argv-based execution for native executables and script/tool
 *   projects without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_RUN_PLAN_H
#define UMICOM_DEVELOPER_PROJECT_RUN_PLAN_H

#include "umicom/build/types.h"
#include "umicom/developer_project/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the developer project run plan data shared with callers of this public
 * contract.
 */
typedef struct UmiDeveloperProjectRunPlan {
    UmiBuildCommand command;
    char language_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    uint32_t timeout_ms;
    int debugger_eligible;
    uint64_t revision;
} UmiDeveloperProjectRunPlan;

/**
 * Initialise developer project run plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_developer_project_run_plan_init(
    UmiDeveloperProjectRunPlan *plan);

/**
 * Provide the developer project run plan native operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_run_plan_native(
    UmiDeveloperProjectRunPlan *plan,
    const char *language_id,
    const char *executable,
    const char *working_directory);

/**
 * Provide the developer project run plan interpreter operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_run_plan_interpreter(
    UmiDeveloperProjectRunPlan *plan,
    const char *language_id,
    const char *interpreter,
    const char *entry_file,
    const char *working_directory);

/**
 * Provide the developer project run plan cargo operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_run_plan_cargo(
    UmiDeveloperProjectRunPlan *plan,
    const char *working_directory,
    int release);

/**
 * Provide the developer project run plan zig operation used by this module and its client
 * applications.
 */
UmiStatus umi_developer_project_run_plan_zig(
    UmiDeveloperProjectRunPlan *plan,
    const char *working_directory);

#ifdef __cplusplus
}
#endif

#endif
