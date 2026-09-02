/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/workflow_types.h
 *
 * PURPOSE:
 *   Define the small set of safe Git workflow actions shared by the Umicom
 *   command, Studio and future applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_WORKFLOW_TYPES_H
#define UMICOM_REPOSITORY_WORKFLOW_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named repository workflow action values accepted by this public contract.
 */
typedef enum UmiRepositoryWorkflowAction {
    UMI_REPOSITORY_WORKFLOW_UNKNOWN = 0,
    UMI_REPOSITORY_WORKFLOW_CLONE = 1,
    UMI_REPOSITORY_WORKFLOW_INITIALISE = 2,
    UMI_REPOSITORY_WORKFLOW_SUBMODULE_ADD = 3,
    UMI_REPOSITORY_WORKFLOW_STAGE = 4,
    UMI_REPOSITORY_WORKFLOW_COMMIT = 5,
    UMI_REPOSITORY_WORKFLOW_PUSH = 6,
    UMI_REPOSITORY_WORKFLOW_PUBLISH = 7,
    /* Appended to preserve the established numeric values above. */
    UMI_REPOSITORY_WORKFLOW_UPDATE = 8
} UmiRepositoryWorkflowAction;

/**
 * Provide the repository workflow action text operation used by this module and its client
 * applications.
 */
const char *umi_repository_workflow_action_text(
    UmiRepositoryWorkflowAction action);
/**
 * Provide the repository workflow action writes index operation used by this module and
 * its client applications.
 */
int umi_repository_workflow_action_writes_index(
    UmiRepositoryWorkflowAction action);
/**
 * Provide the repository workflow action uses remote operation used by this module and its
 * client applications.
 */
int umi_repository_workflow_action_uses_remote(
    UmiRepositoryWorkflowAction action);

#ifdef __cplusplus
}
#endif

#endif
