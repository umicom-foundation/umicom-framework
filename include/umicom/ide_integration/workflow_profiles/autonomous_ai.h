/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ide_integration/workflow_profiles/autonomous_ai.h
 *
 * PURPOSE:
 *   Autonomous AI repair permits dirty working state but blocks unresolved approvals and concurrent debug control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_IDE_WORKFLOW_PROFILE_AUTONOMOUS_AI_H
#define UMICOM_IDE_WORKFLOW_PROFILE_AUTONOMOUS_AI_H
#include "umicom/ide_integration/workflow_policy.h"
#ifdef __cplusplus
extern "C" {
#endif
UmiStatus umi_ide_workflow_profile_autonomous_ai(
    UmiIdeWorkflowPolicy *out_policy);
#ifdef __cplusplus
}
#endif
#endif
