/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/workbench_bridge.h
 *
 * PURPOSE:
 *   Bridge Framework New Project/project models into the professional developer
 *   workbench without introducing project-generation logic into Umicom Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_WORKBENCH_BRIDGE_H
#define UMICOM_DEVELOPER_PROJECT_WORKBENCH_BRIDGE_H

#include "umicom/developer_project/service.h"
#include "umicom/developer_workbench/project_wizard.h"
#include "umicom/developer_workbench/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project request from wizard operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_request_from_wizard(
    const UmiDeveloperWorkbenchProjectWizard *wizard,
    UmiDeveloperProjectGenerationRequest *out_request);

/**
 * Provide the developer project generate from wizard operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_project_generate_from_wizard(
    UmiDeveloperProjectService *projects,
    const UmiDeveloperWorkbenchProjectWizard *wizard,
    int dry_run,
    UmiDeveloperProjectGeneratorReport *out_report,
    UmiDeveloperProjectModel *out_model);

/**
 * Provide the developer project model to workbench configuration operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_project_model_to_workbench_configuration(
    const UmiDeveloperProjectModel *model,
    UmiDeveloperWorkbenchConfiguration *out_configuration);

/**
 * Provide the developer project workbench adopt model operation used by this module and
 * its client applications.
 */
UmiStatus umi_developer_project_workbench_adopt_model(
    UmiDeveloperWorkbench *workbench,
    const UmiDeveloperProjectModel *model,
    int make_active);

#ifdef __cplusplus
}
#endif

#endif
