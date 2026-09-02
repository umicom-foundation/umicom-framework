/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/workflow_validation.h
 *
 * PURPOSE:
 *   Validate repository workflow inputs before Git or the filesystem can be
 *   changed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_WORKFLOW_VALIDATION_H
#define UMICOM_REPOSITORY_WORKFLOW_VALIDATION_H

#include "umicom/base/status.h"
#include "umicom/repository/workflow_request.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Check that repository workflow satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_repository_workflow_validate(
    const UmiRepositoryWorkflowRequest *request);

#ifdef __cplusplus
}
#endif

#endif
