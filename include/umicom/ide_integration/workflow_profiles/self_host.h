/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_profiles/self_host.h
 *
 * PURPOSE:
 *   Self-host readiness requires clean validated state before using the IDE to build itself.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_WORKFLOW_PROFILE_SELF_HOST_H
#define UMICOM_IDE_WORKFLOW_PROFILE_SELF_HOST_H
#include "umicom/ide_integration/workflow_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_ide_workflow_profile_self_host(
    UmiIdeWorkflowPolicy *out_policy);
#ifdef __cplusplus
}
#endif
#endif
