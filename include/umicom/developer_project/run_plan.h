/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/run_plan.h
 *
 * PURPOSE:
 *   Plan safe argv-based execution for native executables and script/tool
 *   projects without shell interpolation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_RUN_PLAN_H
#define UMICOM_DEVELOPER_PROJECT_RUN_PLAN_H

#include "umicom/build/types.h"
#include "umicom/developer_project/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeveloperProjectRunPlan {
    UmiBuildCommand command;
    char language_id[UMI_DEVELOPER_PROJECT_ID_CAPACITY];
    uint32_t timeout_ms;
    int debugger_eligible;
    uint64_t revision;
} UmiDeveloperProjectRunPlan;

void umi_developer_project_run_plan_init(
    UmiDeveloperProjectRunPlan *plan);

UmiStatus umi_developer_project_run_plan_native(
    UmiDeveloperProjectRunPlan *plan,
    const char *language_id,
    const char *executable,
    const char *working_directory);

UmiStatus umi_developer_project_run_plan_interpreter(
    UmiDeveloperProjectRunPlan *plan,
    const char *language_id,
    const char *interpreter,
    const char *entry_file,
    const char *working_directory);

UmiStatus umi_developer_project_run_plan_cargo(
    UmiDeveloperProjectRunPlan *plan,
    const char *working_directory,
    int release);

UmiStatus umi_developer_project_run_plan_zig(
    UmiDeveloperProjectRunPlan *plan,
    const char *working_directory);

#ifdef __cplusplus
}
#endif

#endif
