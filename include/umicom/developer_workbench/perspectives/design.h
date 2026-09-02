/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/perspectives/design.h
 *
 * PURPOSE:
 *   Publish the reusable Design developer perspective.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_DESIGN_H
#define UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_DESIGN_H

#include "umicom/developer_workbench/perspective.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench perspective design operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_design(void);

#ifdef __cplusplus
}
#endif

#endif
