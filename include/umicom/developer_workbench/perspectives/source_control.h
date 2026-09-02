/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/perspectives/source_control.h
 *
 * PURPOSE:
 *   Publish the reusable Source Control developer perspective.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_SOURCE_CONTROL_H
#define UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_SOURCE_CONTROL_H

#include "umicom/developer_workbench/perspective.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench perspective source control operation used by this module
 * and its client applications.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_source_control(void);

#ifdef __cplusplus
}
#endif

#endif
