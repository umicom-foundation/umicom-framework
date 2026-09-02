/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/types.h
 *
 * PURPOSE:
 *   Define bounded operational types for controlled AI developer-tool calls,
 *   plans, results and audit state.
 *
 * ARCHITECTURE:
 *   Tools are Framework-owned capabilities. Models may request a tool call but
 *   cannot bypass the central policy, approval and service-binding boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_TYPES_H
#define UMICOM_AI_CODING_TOOLS_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ai/runtime.h"
#include "umicom/ai_coding_runtime/platform.h"
#include "umicom/debug_runtime/platform.h"
#include "umicom/developer_productivity/source_control.h"
#include "umicom/language/service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_TOOLS_API_VERSION 1U
#define UMI_AI_CODING_TOOL_ID_CAPACITY 128U
#define UMI_AI_CODING_TOOL_TEXT_CAPACITY 8192U
#define UMI_AI_CODING_TOOL_JSON_CAPACITY 32768U
#define UMI_AI_CODING_TOOL_PLAN_CAPACITY 32U
#define UMI_AI_CODING_TOOL_RESULT_HISTORY_CAPACITY 128U
#define UMI_AI_CODING_TOOL_CHECKPOINT_CAPACITY 16U
#define UMI_AI_CODING_TOOL_CHECKPOINT_FILE_CAPACITY 16U
#define UMI_AI_CODING_TOOL_MAX_SEARCH_RESULTS 128U
#define UMI_AI_CODING_TOOL_MAX_FILE_RESULTS 256U
#define UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES 32768U

/**
 * List the named ai coding tool risk values accepted by this public contract.
 */
typedef enum UmiAiCodingToolRisk {
    UMI_AI_CODING_TOOL_RISK_READ_ONLY = 0,
    UMI_AI_CODING_TOOL_RISK_LOCAL_MUTATION = 1,
    UMI_AI_CODING_TOOL_RISK_PROCESS_EXECUTION = 2,
    UMI_AI_CODING_TOOL_RISK_SOURCE_CONTROL_MUTATION = 3,
    UMI_AI_CODING_TOOL_RISK_NETWORK = 4,
    UMI_AI_CODING_TOOL_RISK_DEBUG_CONTROL = 5
} UmiAiCodingToolRisk;

/**
 * List the named ai coding tool capability values accepted by this public contract.
 */
typedef enum UmiAiCodingToolCapability {
    UMI_AI_CODING_TOOL_CAP_WORKSPACE_READ = UINT64_C(1) << 0,
    UMI_AI_CODING_TOOL_CAP_WORKSPACE_SEARCH = UINT64_C(1) << 1,
    UMI_AI_CODING_TOOL_CAP_LANGUAGE_READ = UINT64_C(1) << 2,
    UMI_AI_CODING_TOOL_CAP_DIAGNOSTIC_READ = UINT64_C(1) << 3,
    UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_READ = UINT64_C(1) << 4,
    UMI_AI_CODING_TOOL_CAP_SOURCE_CONTROL_WRITE = UINT64_C(1) << 5,
    UMI_AI_CODING_TOOL_CAP_PROCESS_EXECUTION = UINT64_C(1) << 6,
    UMI_AI_CODING_TOOL_CAP_DEBUG_READ = UINT64_C(1) << 7,
    UMI_AI_CODING_TOOL_CAP_DEBUG_CONTROL = UINT64_C(1) << 8,
    UMI_AI_CODING_TOOL_CAP_AGENT_READ = UINT64_C(1) << 9,
    UMI_AI_CODING_TOOL_CAP_AGENT_CONTROL = UINT64_C(1) << 10,
    UMI_AI_CODING_TOOL_CAP_CHECKPOINT = UINT64_C(1) << 11,
    UMI_AI_CODING_TOOL_CAP_NETWORK = UINT64_C(1) << 12
} UmiAiCodingToolCapability;

/**
 * List the named ai coding tool call state values accepted by this public contract.
 */
typedef enum UmiAiCodingToolCallState {
    UMI_AI_CODING_TOOL_CALL_PENDING = 0,
    UMI_AI_CODING_TOOL_CALL_APPROVAL_REQUIRED = 1,
    UMI_AI_CODING_TOOL_CALL_RUNNING = 2,
    UMI_AI_CODING_TOOL_CALL_SUCCEEDED = 3,
    UMI_AI_CODING_TOOL_CALL_FAILED = 4,
    UMI_AI_CODING_TOOL_CALL_REJECTED = 5,
    UMI_AI_CODING_TOOL_CALL_CANCELLED = 6
} UmiAiCodingToolCallState;

/**
 * Represent the ai coding tool descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingToolDescriptor {
    char tool_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    char label[256];
    char description[1024];
    char permission[UMI_AI_ID_CAPACITY];
    UmiAiCodingToolRisk risk;
    uint64_t required_capabilities;
    int requires_approval;
    int enabled;
    uint64_t revision;
} UmiAiCodingToolDescriptor;

/**
 * Represent the ai coding tool call data shared with callers of this public contract.
 */
typedef struct UmiAiCodingToolCall {
    uint64_t call_id;
    char tool_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    char arguments_json[UMI_AI_CODING_TOOL_JSON_CAPACITY];
    UmiAiCodingToolCallState state;
    UmiStatus status;
    uint64_t sequence;
    int approved;
} UmiAiCodingToolCall;

/**
 * Represent the ai coding tool result data shared with callers of this public contract.
 */
typedef struct UmiAiCodingToolResult {
    uint64_t call_id;
    char tool_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    UmiStatus status;
    UmiAiCodingToolCallState state;
    char output[UMI_AI_CODING_TOOL_MAX_OUTPUT_BYTES];
    size_t output_length;
    uint64_t sequence;
    int output_truncated;
} UmiAiCodingToolResult;

/**
 * Represent the ai coding tool plan step data shared with callers of this public contract.
 */
typedef struct UmiAiCodingToolPlanStep {
    char step_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    UmiAiCodingToolCall call;
    size_t depends_on_index;
    int has_dependency;
    int required;
    int continue_on_failure;
} UmiAiCodingToolPlanStep;

/**
 * Represent the ai coding tool plan data shared with callers of this public contract.
 */
typedef struct UmiAiCodingToolPlan {
    char plan_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    char title[256];
    char rationale[2048];
    UmiAiCodingToolPlanStep steps[UMI_AI_CODING_TOOL_PLAN_CAPACITY];
    size_t step_count;
    uint64_t revision;
} UmiAiCodingToolPlan;

/**
 * Represent the ai coding tool plan result data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingToolPlanResult {
    char plan_id[UMI_AI_CODING_TOOL_ID_CAPACITY];
    UmiAiCodingToolResult results[UMI_AI_CODING_TOOL_PLAN_CAPACITY];
    size_t result_count;
    size_t passed_count;
    size_t failed_count;
    size_t rejected_count;
    uint64_t revision;
    int completed;
    int succeeded;
} UmiAiCodingToolPlanResult;

/**
 * Provide the ai coding tool risk text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_tool_risk_text(UmiAiCodingToolRisk risk);
/**
 * Provide the ai coding tool call state text operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_tool_call_state_text(UmiAiCodingToolCallState state);

#ifdef __cplusplus
}
#endif
#endif
