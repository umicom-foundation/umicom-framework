/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/types.h
 *
 * PURPOSE:
 *   Define bounded toolkit-neutral records for the AI developer experience:
 *   chats, tasks, approvals, patch review, diff navigation and durable UI state.
 *
 * ARCHITECTURE:
 *   These records project existing AI Coding Runtime and Coding Tools state.
 *   They do not execute providers, mutate repositories or own toolkit widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TYPES_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ai_coding_runtime/ai_coding_runtime.h"
#include "umicom/ai_coding_tools/ai_coding_tools.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_DEVELOPER_EXPERIENCE_API_VERSION 1U
#define UMI_AI_DEVELOPER_ID_CAPACITY 128U
#define UMI_AI_DEVELOPER_TITLE_CAPACITY 256U
#define UMI_AI_DEVELOPER_TEXT_CAPACITY 4096U
#define UMI_AI_DEVELOPER_PATH_CAPACITY 2048U
#define UMI_AI_DEVELOPER_APPROVAL_CAPACITY 64U
#define UMI_AI_DEVELOPER_TASK_CAPACITY 64U
#define UMI_AI_DEVELOPER_TIMELINE_CAPACITY 256U
#define UMI_AI_DEVELOPER_DIFF_LINE_CAPACITY 512U
#define UMI_AI_DEVELOPER_DIFF_HUNK_CAPACITY 64U
#define UMI_AI_DEVELOPER_REVIEW_FILE_CAPACITY UMI_AI_CODING_PATCH_FILE_MAX
#define UMI_AI_DEVELOPER_VISIBLE_ROW_CAPACITY 64U

/**
 * List the named ai developer pane kind values accepted by this public contract.
 */
typedef enum UmiAiDeveloperPaneKind {
    UMI_AI_DEVELOPER_PANE_OVERVIEW = 1,
    UMI_AI_DEVELOPER_PANE_CHAT = 2,
    UMI_AI_DEVELOPER_PANE_TASKS = 3,
    UMI_AI_DEVELOPER_PANE_APPROVALS = 4,
    UMI_AI_DEVELOPER_PANE_PATCH_REVIEW = 5,
    UMI_AI_DEVELOPER_PANE_DIFF = 6,
    UMI_AI_DEVELOPER_PANE_TOOL_ACTIVITY = 7,
    UMI_AI_DEVELOPER_PANE_VALIDATION = 8,
    UMI_AI_DEVELOPER_PANE_CONTEXT = 9,
    UMI_AI_DEVELOPER_PANE_HISTORY = 10,
    UMI_AI_DEVELOPER_PANE_POLICY = 11,
    UMI_AI_DEVELOPER_PANE_CHECKPOINTS = 12
} UmiAiDeveloperPaneKind;

/**
 * List the named ai developer approval kind values accepted by this public contract.
 */
typedef enum UmiAiDeveloperApprovalKind {
    UMI_AI_DEVELOPER_APPROVAL_TOOL_CALL = 1,
    UMI_AI_DEVELOPER_APPROVAL_PATCH = 2,
    UMI_AI_DEVELOPER_APPROVAL_CHECKPOINT_RESTORE = 3,
    UMI_AI_DEVELOPER_APPROVAL_SOURCE_CONTROL = 4,
    UMI_AI_DEVELOPER_APPROVAL_DEBUG_CONTROL = 5
} UmiAiDeveloperApprovalKind;

/**
 * List the named ai developer approval state values accepted by this public contract.
 */
typedef enum UmiAiDeveloperApprovalState {
    UMI_AI_DEVELOPER_APPROVAL_PENDING = 0,
    UMI_AI_DEVELOPER_APPROVAL_APPROVED = 1,
    UMI_AI_DEVELOPER_APPROVAL_REJECTED = 2,
    UMI_AI_DEVELOPER_APPROVAL_EXECUTED = 3,
    UMI_AI_DEVELOPER_APPROVAL_FAILED = 4
} UmiAiDeveloperApprovalState;

/**
 * List the named ai developer task state values accepted by this public contract.
 */
typedef enum UmiAiDeveloperTaskState {
    UMI_AI_DEVELOPER_TASK_QUEUED = 0,
    UMI_AI_DEVELOPER_TASK_RUNNING = 1,
    UMI_AI_DEVELOPER_TASK_REVIEW = 2,
    UMI_AI_DEVELOPER_TASK_VALIDATING = 3,
    UMI_AI_DEVELOPER_TASK_COMPLETED = 4,
    UMI_AI_DEVELOPER_TASK_FAILED = 5,
    UMI_AI_DEVELOPER_TASK_CANCELLED = 6
} UmiAiDeveloperTaskState;

/**
 * List the named ai developer diff line kind values accepted by this public contract.
 */
typedef enum UmiAiDeveloperDiffLineKind {
    UMI_AI_DEVELOPER_DIFF_CONTEXT = 0,
    UMI_AI_DEVELOPER_DIFF_ADD = 1,
    UMI_AI_DEVELOPER_DIFF_REMOVE = 2
} UmiAiDeveloperDiffLineKind;

/**
 * Represent the ai developer approval request data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperApprovalRequest {
    char approval_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    UmiAiDeveloperApprovalKind kind;
    UmiAiDeveloperApprovalState state;
    char title[UMI_AI_DEVELOPER_TITLE_CAPACITY];
    char summary[UMI_AI_DEVELOPER_TEXT_CAPACITY];
    char permission[UMI_AI_ID_CAPACITY];
    char subject_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    UmiAiCodingToolRisk risk;
    UmiAiCodingToolCall tool_call;
    uint64_t requested_sequence;
    uint64_t decided_sequence;
    int executable;
} UmiAiDeveloperApprovalRequest;

/**
 * Represent the ai developer task entry data shared with callers of this public contract.
 */
typedef struct UmiAiDeveloperTaskEntry {
    char task_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    char request_id[UMI_AI_ID_CAPACITY];
    char title[UMI_AI_DEVELOPER_TITLE_CAPACITY];
    char summary[UMI_AI_DEVELOPER_TEXT_CAPACITY];
    UmiAiDeveloperTaskState state;
    uint32_t iteration;
    uint32_t maximum_iterations;
    size_t patch_files;
    size_t validation_failures;
    uint64_t created_sequence;
    uint64_t updated_sequence;
} UmiAiDeveloperTaskEntry;

/**
 * Represent the ai developer timeline event data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperTimelineEvent {
    uint64_t sequence;
    char task_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    char category[64];
    char title[UMI_AI_DEVELOPER_TITLE_CAPACITY];
    char detail[UMI_AI_DEVELOPER_TEXT_CAPACITY];
    UmiStatus status;
    int important;
} UmiAiDeveloperTimelineEvent;

/**
 * Represent the ai developer presentation state data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperPresentationState {
    UmiAiDeveloperPaneKind active_pane;
    char active_chat_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    char active_task_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    char active_approval_id[UMI_AI_DEVELOPER_ID_CAPACITY];
    char active_patch_id[UMI_AI_ID_CAPACITY];
    char active_diff_path[UMI_AI_DEVELOPER_PATH_CAPACITY];
    size_t active_diff_line;
    size_t selected_context_row;
    int show_tool_details;
    int show_validation_output;
    uint64_t revision;
} UmiAiDeveloperPresentationState;

/**
 * Provide the ai developer pane text operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_pane_text(UmiAiDeveloperPaneKind kind);
/**
 * Provide the ai developer approval kind text operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_approval_kind_text(UmiAiDeveloperApprovalKind kind);
/**
 * Provide the ai developer approval state text operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_approval_state_text(UmiAiDeveloperApprovalState state);
/**
 * Provide the ai developer task state text operation used by this module and its client
 * applications.
 */
const char *umi_ai_developer_task_state_text(UmiAiDeveloperTaskState state);
/**
 * Provide the ai developer diff line kind text operation used by this module and its
 * client applications.
 */
const char *umi_ai_developer_diff_line_kind_text(UmiAiDeveloperDiffLineKind kind);

#ifdef __cplusplus
}
#endif
#endif
