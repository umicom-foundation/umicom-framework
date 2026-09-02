/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/workflow_request.h
 *
 * PURPOSE:
 *   Describe one repository workflow without coupling reusable Framework code
 *   to command-line argument parsing or a particular user interface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_WORKFLOW_REQUEST_H
#define UMICOM_REPOSITORY_WORKFLOW_REQUEST_H

#include "umicom/repository/workflow_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the repository workflow request data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryWorkflowRequest {
    UmiRepositoryWorkflowAction action;
    const char *repository_root;
    const char *source_url;
    const char *destination;
    const char *submodule_path;
    const char *branch;
    const char *remote_name;
    const char *remote_url;
    const char *commit_message;
    unsigned clone_depth;
    int recursive;
    int set_upstream;
    int dry_run;
} UmiRepositoryWorkflowRequest;

/**
 * Initialise repository workflow request from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_workflow_request_init(
    UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowAction action,
    const char *repository_root);

#ifdef __cplusplus
}
#endif

#endif
