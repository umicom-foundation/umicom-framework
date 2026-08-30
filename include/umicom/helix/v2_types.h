/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/v2_types.h
 * PURPOSE: Define deterministic Helix Agent Runtime v2 roles, actions and states.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE:
 * These enums make every autonomous step visible and prevent an operation
 * from skipping directly from planning to an unreviewed source change.
 */
#ifndef INCLUDE_UMICOM_HELIX_V2_TYPES_H
#define INCLUDE_UMICOM_HELIX_V2_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_HELIX_V2_MAX_AGENTS 5U
#define UMI_HELIX_V2_MAX_ACTIONS 32U
#define UMI_HELIX_V2_MAX_APPROVALS 32U
#define UMI_HELIX_V2_MAX_JOURNAL_ENTRIES 128U
#define UMI_HELIX_V2_MAX_CANDIDATES 16U
#define UMI_HELIX_V2_MAX_COMPENSATIONS 16U
#define UMI_HELIX_V2_HASH_CAPACITY 32U

typedef enum UmiHelixAgentRoleV2 {
    UMI_HELIX_AGENT_ROLE_DESIGNER = 1,
    UMI_HELIX_AGENT_ROLE_BUILDER = 2,
    UMI_HELIX_AGENT_ROLE_SUGGESTION = 3,
    UMI_HELIX_AGENT_ROLE_REVIEWER = 4,
    UMI_HELIX_AGENT_ROLE_TEST = 5
} UmiHelixAgentRoleV2;

typedef enum UmiHelixActionKindV2 {
    UMI_HELIX_ACTION_READ = 1,
    UMI_HELIX_ACTION_FILESYSTEM = 2,
    UMI_HELIX_ACTION_BUILD = 3,
    UMI_HELIX_ACTION_TEST = 4,
    UMI_HELIX_ACTION_REVIEW = 5,
    UMI_HELIX_ACTION_SOURCE_CONTROL = 6
} UmiHelixActionKindV2;

typedef enum UmiHelixOperationStateV2 {
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
} UmiHelixOperationStateV2;

const char *umi_helix_agent_role_v2_text(UmiHelixAgentRoleV2 role);
const char *umi_helix_action_kind_v2_text(UmiHelixActionKindV2 kind);
const char *umi_helix_operation_state_v2_text(UmiHelixOperationStateV2 state);
int umi_helix_action_kind_v2_is_mutating(UmiHelixActionKindV2 kind);
int umi_helix_operation_state_v2_can_transition(
    UmiHelixOperationStateV2 current,
    UmiHelixOperationStateV2 next);

#ifdef __cplusplus
}
#endif

#endif
