/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/workbench_bridge.h
 *
 * PURPOSE:
 *   Bridge Framework New Project/project models into the professional developer
 *   workbench without introducing project-generation logic into Umicom Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_WORKBENCH_BRIDGE_H
#define UMICOM_DEVELOPER_PROJECT_WORKBENCH_BRIDGE_H

#include "umicom/developer_project/service.h"
#include "umicom/developer_workbench/project_wizard.h"
#include "umicom/developer_workbench/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_developer_project_request_from_wizard(
    const UmiDeveloperWorkbenchProjectWizard *wizard,
    UmiDeveloperProjectGenerationRequest *out_request);

UmiStatus umi_developer_project_generate_from_wizard(
    UmiDeveloperProjectService *projects,
    const UmiDeveloperWorkbenchProjectWizard *wizard,
    int dry_run,
    UmiDeveloperProjectGeneratorReport *out_report,
    UmiDeveloperProjectModel *out_model);

UmiStatus umi_developer_project_model_to_workbench_configuration(
    const UmiDeveloperProjectModel *model,
    UmiDeveloperWorkbenchConfiguration *out_configuration);

UmiStatus umi_developer_project_workbench_adopt_model(
    UmiDeveloperWorkbench *workbench,
    const UmiDeveloperProjectModel *model,
    int make_active);

#ifdef __cplusplus
}
#endif

#endif
