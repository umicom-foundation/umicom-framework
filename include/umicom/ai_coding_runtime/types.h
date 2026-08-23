/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/types.h
 *
 * PURPOSE:
 *   Define bounded, provider-neutral operational types for repository-aware
 *   AI coding, validation and agentic edit workflows.
 *
 * ARCHITECTURE:
 *   The established umicom/ai coding-assistant contracts remain authoritative
 *   for planning, privacy and patch approval.  These records add execution,
 *   iteration, validation and observable runtime state without exposing an IDE
 *   toolkit or a provider-specific SDK.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_TYPES_H
#define UMICOM_AI_CODING_RUNTIME_TYPES_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/ai/coding_assistant.h"
#include "umicom/ai/runtime.h"
#include "umicom/developer/executor.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_CODING_RUNTIME_API_VERSION 1U
#define UMI_AI_CODING_RUNTIME_ID_CAPACITY 128U
#define UMI_AI_CODING_RUNTIME_PATH_CAPACITY 2048U
#define UMI_AI_CODING_RUNTIME_TEXT_CAPACITY 8192U
#define UMI_AI_CODING_RUNTIME_PROMPT_CAPACITY 32768U
#define UMI_AI_CODING_RUNTIME_RESPONSE_CAPACITY 32768U
#define UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY 24U
#define UMI_AI_CODING_RUNTIME_VALIDATION_CAPACITY 16U
#define UMI_AI_CODING_RUNTIME_EVENT_CAPACITY 128U
#define UMI_AI_CODING_RUNTIME_HISTORY_CAPACITY 64U
#define UMI_AI_CODING_RUNTIME_MAX_ITERATIONS 8U

typedef enum UmiAiCodingRuntimeState {
    UMI_AI_CODING_RUNTIME_IDLE = 0,
    UMI_AI_CODING_RUNTIME_PREPARING = 1,
    UMI_AI_CODING_RUNTIME_MATERIALISING_CONTEXT = 2,
    UMI_AI_CODING_RUNTIME_GENERATING = 3,
    UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED = 4,
    UMI_AI_CODING_RUNTIME_APPLYING = 5,
    UMI_AI_CODING_RUNTIME_VALIDATING = 6,
    UMI_AI_CODING_RUNTIME_REPAIRING = 7,
    UMI_AI_CODING_RUNTIME_COMPLETED = 8,
    UMI_AI_CODING_RUNTIME_CANCELLED = 9,
    UMI_AI_CODING_RUNTIME_FAILED = 10
} UmiAiCodingRuntimeState;

typedef enum UmiAiCodingValidationKind {
    UMI_AI_CODING_VALIDATION_CUSTOM = 0,
    UMI_AI_CODING_VALIDATION_CONFIGURE = 1,
    UMI_AI_CODING_VALIDATION_BUILD = 2,
    UMI_AI_CODING_VALIDATION_TEST = 3,
    UMI_AI_CODING_VALIDATION_LINT = 4,
    UMI_AI_CODING_VALIDATION_FORMAT_CHECK = 5,
    UMI_AI_CODING_VALIDATION_PACKAGE = 6
} UmiAiCodingValidationKind;

typedef enum UmiAiCodingEventKind {
    UMI_AI_CODING_EVENT_STATE = 1,
    UMI_AI_CODING_EVENT_CONTEXT = 2,
    UMI_AI_CODING_EVENT_PROVIDER = 3,
    UMI_AI_CODING_EVENT_PATCH = 4,
    UMI_AI_CODING_EVENT_VALIDATION = 5,
    UMI_AI_CODING_EVENT_ROLLBACK = 6,
    UMI_AI_CODING_EVENT_MESSAGE = 7
} UmiAiCodingEventKind;

typedef struct UmiAiCodingMaterializedFile {
    char path[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    char language_id[UMI_AI_ID_CAPACITY];
    char text[UMI_AI_CODING_PATCH_CONTENT_CAPACITY];
    size_t length;
    uint64_t hash;
    uint32_t estimated_tokens;
    int active;
} UmiAiCodingMaterializedFile;

typedef struct UmiAiCodingMaterializedContext {
    UmiAiCodingMaterializedFile files[UMI_AI_CODING_RUNTIME_CONTEXT_CAPACITY];
    size_t file_count;
    size_t skipped_count;
    uint32_t estimated_tokens;
    uint64_t revision;
    int truncated;
} UmiAiCodingMaterializedContext;

typedef struct UmiAiCodingValidationStep {
    char id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    UmiAiCodingValidationKind kind;
    char label[256];
    char executable[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    char arguments[UMI_AI_CODING_RUNTIME_TEXT_CAPACITY];
    char working_directory[UMI_AI_CODING_RUNTIME_PATH_CAPACITY];
    int required;
    int enabled;
    uint32_t timeout_ms;
} UmiAiCodingValidationStep;

typedef struct UmiAiCodingValidationResult {
    char step_id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    UmiAiCodingValidationKind kind;
    UmiStatus status;
    int exit_code;
    int passed;
    int required;
    int output_truncated;
    uint64_t duration_ms;
    char output[UMI_DEVELOPER_EXECUTION_OUTPUT_CAPACITY];
} UmiAiCodingValidationResult;

typedef struct UmiAiCodingValidationReport {
    UmiAiCodingValidationResult
        results[UMI_AI_CODING_RUNTIME_VALIDATION_CAPACITY];
    size_t result_count;
    size_t passed_count;
    size_t failed_count;
    size_t required_failed_count;
    uint64_t revision;
    int passed;
} UmiAiCodingValidationReport;

typedef struct UmiAiCodingEvent {
    uint64_t sequence;
    UmiAiCodingEventKind kind;
    UmiAiCodingRuntimeState state;
    UmiStatus status;
    char task_id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    char message[1024];
} UmiAiCodingEvent;

typedef struct UmiAiCodingAgentSnapshot {
    char task_id[UMI_AI_CODING_RUNTIME_ID_CAPACITY];
    char request_id[UMI_AI_ID_CAPACITY];
    char provider_id[UMI_AI_ID_CAPACITY];
    char model_id[UMI_AI_ID_CAPACITY];
    char patch_id[UMI_AI_ID_CAPACITY];
    UmiAiCodingRuntimeState state;
    UmiStatus last_status;
    uint32_t iteration;
    uint32_t maximum_iterations;
    size_t context_files;
    size_t patch_files;
    size_t validation_steps;
    size_t validation_failures;
    uint64_t revision;
    int cancelled;
    int patch_recorded;
    int patch_applied;
    int validation_passed;
    int rolled_back;
} UmiAiCodingAgentSnapshot;

const char *umi_ai_coding_runtime_state_text(UmiAiCodingRuntimeState state);
const char *umi_ai_coding_validation_kind_text(UmiAiCodingValidationKind kind);
const char *umi_ai_coding_event_kind_text(UmiAiCodingEventKind kind);

#ifdef __cplusplus
}
#endif
#endif
