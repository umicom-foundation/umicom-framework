/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_profiles/release.h
 *
 * PURPOSE:
 *   Release readiness requires clean source control and complete test/diagnostic/debug/AI gates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_WORKFLOW_PROFILE_RELEASE_H
#define UMICOM_IDE_WORKFLOW_PROFILE_RELEASE_H
#include "umicom/ide_integration/workflow_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Release or reset state held by ide workflow profile so the same storage can be reused
 * safely.
 */
UmiStatus umi_ide_workflow_profile_release(
    UmiIdeWorkflowPolicy *out_policy);
#ifdef __cplusplus
}
#endif
#endif
