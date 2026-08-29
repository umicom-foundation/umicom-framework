/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/workflow_executor.h
 *
 * PURPOSE:
 *   Execute safe, argument-based Git repository workflows using a discovered
 *   toolchain and an isolated child-process environment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_WORKFLOW_EXECUTOR_H
#define UMICOM_REPOSITORY_WORKFLOW_EXECUTOR_H

#include "umicom/repository/workflow_report.h"
#include "umicom/repository/workflow_request.h"
#include "umicom/toolchain/environment.h"
#include "umicom/toolchain/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_repository_workflow_execute(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowReport *out_report);

#ifdef __cplusplus
}
#endif

#endif
