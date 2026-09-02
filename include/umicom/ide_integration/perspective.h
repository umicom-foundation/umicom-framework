/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/perspective.h
 *
 * PURPOSE:
 *   Publish the integrated development workbench perspective built from existing
 *   Framework surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_INTEGRATION_PERSPECTIVE_H
#define UMICOM_IDE_INTEGRATION_PERSPECTIVE_H
#include "umicom/developer_workbench/perspective.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide integrated development perspective operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_ide_integrated_development_perspective(void);
#ifdef __cplusplus
}
#endif
#endif
