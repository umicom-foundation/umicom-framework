/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_profiles/development.h
 *
 * PURPOSE:
 *   Interactive development permits dirty files and active debug sessions while surfacing warnings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_WORKFLOW_PROFILE_DEVELOPMENT_H
#define UMICOM_IDE_WORKFLOW_PROFILE_DEVELOPMENT_H
#include "umicom/ide_integration/workflow_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the ide workflow profile development operation used by this module and its
 * client applications.
 */
UmiStatus umi_ide_workflow_profile_development(
    UmiIdeWorkflowPolicy *out_policy);
#ifdef __cplusplus
}
#endif
#endif
