/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_workbench/perspectives/testing.h
 *
 * PURPOSE:
 *   Publish the reusable Testing developer perspective.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_TESTING_H
#define UMICOM_DEVELOPER_WORKBENCH_PERSPECTIVE_TESTING_H

#include "umicom/developer_workbench/perspective.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the developer workbench perspective testing operation used by this module and
 * its client applications.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_testing(void);

#ifdef __cplusplus
}
#endif

#endif
