/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_profiles/release.h
 *
 * PURPOSE:
 *   Release readiness requires clean source control and complete test/diagnostic/debug/AI gates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_WORKFLOW_PROFILE_RELEASE_H
#define UMICOM_IDE_WORKFLOW_PROFILE_RELEASE_H
#include "umicom/ide_integration/workflow_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_ide_workflow_profile_release(
    UmiIdeWorkflowPolicy *out_policy);
#ifdef __cplusplus
}
#endif
#endif
