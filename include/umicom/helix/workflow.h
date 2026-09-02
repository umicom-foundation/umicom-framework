/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/workflow.h
 *
 * PURPOSE:
 *   Track the ordered phases of one Helix candidate workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The workflow enforces a simple forward lifecycle so policy can attach gates to known stages rather than arbitrary agent actions.
 */

#ifndef INCLUDE_UMICOM_HELIX_WORKFLOW_H
#define INCLUDE_UMICOM_HELIX_WORKFLOW_H

#include <stddef.h>
#include "umicom/helix/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named helix workflow stage values accepted by this public contract.
 */
typedef enum UmiHelixWorkflowStage {
    UMI_HELIX_STAGE_OBSERVE = 1,
    UMI_HELIX_STAGE_DIAGNOSE = 2,
    UMI_HELIX_STAGE_PLAN = 3,
    UMI_HELIX_STAGE_BUILD = 4,
    UMI_HELIX_STAGE_VALIDATE = 5,
    UMI_HELIX_STAGE_APPROVE = 6,
    UMI_HELIX_STAGE_PROMOTE = 7
} UmiHelixWorkflowStage;

/**
 * Represent the helix workflow data shared with callers of this public contract.
 */
typedef struct UmiHelixWorkflow {
    char workflow_id[UMI_HELIX_ID_CAPACITY];
    UmiHelixWorkflowStage stage;
    int failed;
} UmiHelixWorkflow;

/**
 * Initialise helix workflow from caller-provided values so later operations receive a
 * known state.
 */
void umi_helix_workflow_init(UmiHelixWorkflow *workflow, const char *workflow_id);
/**
 * Provide the helix workflow advance operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_workflow_advance(UmiHelixWorkflow *workflow,
                                     UmiHelixWorkflowStage next_stage);
/**
 * Provide the helix workflow stage text operation used by this module and its client
 * applications.
 */
const char *umi_helix_workflow_stage_text(UmiHelixWorkflowStage stage);

#ifdef __cplusplus
}
#endif

#endif
