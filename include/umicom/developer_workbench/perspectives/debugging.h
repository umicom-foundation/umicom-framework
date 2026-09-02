/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/perspectives/debugging.h
 *
 * PURPOSE:
 *   Publish the reusable Debugging developer perspective.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_DEBUGGING_H
#define UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_DEBUGGING_H

#include "umicom/developer_workbench/perspective.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench perspective debugging operation used by this module and
 * its client applications.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_debugging(void);

#ifdef __cplusplus
}
#endif

#endif
