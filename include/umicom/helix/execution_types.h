/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/execution_types.h
 * PURPOSE: Define deterministic Helix Agent Runtime v2 roles, actions and states.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*
 * These enums make every autonomous step visible and prevent an operation
 * from skipping directly from planning to an unreviewed source change.
 */
#ifndef INCLUDE_UMICOM_HELIX_EXECUTION_TYPES_H
#define INCLUDE_UMICOM_HELIX_EXECUTION_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HELIX_RUNTIME_MAX_AGENTS 5U
#define UMI_HELIX_RUNTIME_MAX_ACTIONS 32U
#define UMI_HELIX_RUNTIME_MAX_APPROVALS 32U
#define UMI_HELIX_RUNTIME_MAX_JOURNAL_ENTRIES 128U
#define UMI_HELIX_RUNTIME_MAX_CANDIDATES 16U
#define UMI_HELIX_RUNTIME_MAX_COMPENSATIONS 16U
#define UMI_HELIX_RUNTIME_HASH_CAPACITY 32U

typedef enum UmiHelixAgentRole {
    UMI_HELIX_AGENT_ROLE_DESIGNER = 1,
    UMI_HELIX_AGENT_ROLE_BUILDER = 2,
    UMI_HELIX_AGENT_ROLE_SUGGESTION = 3,
    UMI_HELIX_AGENT_ROLE_REVIEWER = 4,
    UMI_HELIX_AGENT_ROLE_TEST = 5
} UmiHelixAgentRole;

typedef enum UmiHelixActionKind {
    UMI_HELIX_ACTION_READ = 1,
    UMI_HELIX_ACTION_FILESYSTEM = 2,
    UMI_HELIX_ACTION_BUILD = 3,
    UMI_HELIX_ACTION_TEST = 4,
    UMI_HELIX_ACTION_REVIEW = 5,
    UMI_HELIX_ACTION_SOURCE_CONTROL = 6
} UmiHelixActionKind;

typedef enum UmiHelixOperationState {
    UMI_HELIX_OPERATION_DRAFT = 1,
    UMI_HELIX_OPERATION_PLANNED = 2,
    UMI_HELIX_OPERATION_AWAITING_APPROVAL = 3,
    UMI_HELIX_OPERATION_APPROVED = 4,
    UMI_HELIX_OPERATION_RUNNING = 5,
    UMI_HELIX_OPERATION_SUCCEEDED = 6,
    UMI_HELIX_OPERATION_FAILED = 7,
    UMI_HELIX_OPERATION_ROLLING_BACK = 8,
    UMI_HELIX_OPERATION_ROLLED_BACK = 9,
    UMI_HELIX_OPERATION_CANCELLED = 10
} UmiHelixOperationState;

const char *umi_helix_agent_role_text(UmiHelixAgentRole role);
const char *umi_helix_action_kind_text(UmiHelixActionKind kind);
const char *umi_helix_operation_state_text(UmiHelixOperationState state);
int umi_helix_action_kind_is_mutating(UmiHelixActionKind kind);
int umi_helix_operation_state_can_transition(
    UmiHelixOperationState current,
    UmiHelixOperationState next);

#ifdef __cplusplus
}
#endif

#endif
