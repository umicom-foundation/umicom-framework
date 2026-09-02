/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_project/templates/integration_worker.h
 *
 * PURPOSE:
 *   Publish the built-in Umicom Integration Worker project template.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_PROJECT_TEMPLATE_INTEGRATION_WORKER_H
#define UMICOM_DEVELOPER_PROJECT_TEMPLATE_INTEGRATION_WORKER_H

#include "umicom/developer_project/template.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer project template integration worker operation used by this module
 * and its client applications.
 */
const UmiDeveloperProjectTemplate *umi_developer_project_template_integration_worker(void);

#ifdef __cplusplus
}
#endif

#endif
