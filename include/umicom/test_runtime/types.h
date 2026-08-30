/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/types.h
 *
 * PURPOSE:
 *   Define stable C23 contracts for test launch, execution evidence and failure classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TYPES_H
#define UMICOM_TEST_RUNTIME_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_RUNTIME_ID_CAPACITY 160U
#define UMI_TEST_RUNTIME_PATH_CAPACITY 1024U
#define UMI_TEST_RUNTIME_TEXT_CAPACITY 512U
#define UMI_TEST_RUNTIME_MAX_ARGUMENTS 32U
#define UMI_TEST_RUNTIME_MAX_ENVIRONMENT 64U
#define UMI_TEST_RUNTIME_MAX_DEPENDENCIES 64U
#define UMI_TEST_RUNTIME_MAX_RECORDS 512U
#define UMI_TEST_RUNTIME_MAX_PROFILES 16U
#define UMI_TEST_RUNTIME_MAX_SHARDS 64U
#define UMI_TEST_RUNTIME_MAX_FAILURES 256U

typedef enum UmiTestRuntimeState {
    UMI_TEST_RUNTIME_STATE_CREATED = 1,
    UMI_TEST_RUNTIME_STATE_READY = 2,
    UMI_TEST_RUNTIME_STATE_STARTING = 3,
    UMI_TEST_RUNTIME_STATE_RUNNING = 4,
    UMI_TEST_RUNTIME_STATE_PASSED = 5,
    UMI_TEST_RUNTIME_STATE_FAILED = 6,
    UMI_TEST_RUNTIME_STATE_NOT_RUN = 7,
    UMI_TEST_RUNTIME_STATE_TIMED_OUT = 8,
    UMI_TEST_RUNTIME_STATE_CRASHED = 9,
    UMI_TEST_RUNTIME_STATE_CANCELLED = 10
} UmiTestRuntimeState;

typedef enum UmiTestRuntimeFailureKind {
    UMI_TEST_RUNTIME_FAILURE_NONE = 0,
    UMI_TEST_RUNTIME_FAILURE_BAD_COMMAND = 1,
    UMI_TEST_RUNTIME_FAILURE_MISSING_EXECUTABLE = 2,
    UMI_TEST_RUNTIME_FAILURE_MISSING_RUNTIME_DEPENDENCY = 3,
    UMI_TEST_RUNTIME_FAILURE_INVALID_WORKING_DIRECTORY = 4,
    UMI_TEST_RUNTIME_FAILURE_TIMEOUT = 5,
    UMI_TEST_RUNTIME_FAILURE_ASSERTION = 6,
    UMI_TEST_RUNTIME_FAILURE_CRASH = 7,
    UMI_TEST_RUNTIME_FAILURE_ACCESS_VIOLATION = 8,
    UMI_TEST_RUNTIME_FAILURE_STACK_OVERFLOW = 9,
    UMI_TEST_RUNTIME_FAILURE_SANITIZER = 10,
    UMI_TEST_RUNTIME_FAILURE_NONZERO_EXIT = 11,
    UMI_TEST_RUNTIME_FAILURE_CONFIGURATION = 12,
    UMI_TEST_RUNTIME_FAILURE_UNKNOWN = 13,
    UMI_TEST_RUNTIME_FAILURE_FAST_FAIL = 14
} UmiTestRuntimeFailureKind;

typedef enum UmiTestRuntimeProfileKind {
    UMI_TEST_RUNTIME_PROFILE_SMOKE = 1,
    UMI_TEST_RUNTIME_PROFILE_FRAMEWORK_CORE = 2,
    UMI_TEST_RUNTIME_PROFILE_LAYOUT = 3,
    UMI_TEST_RUNTIME_PROFILE_DATA = 4,
    UMI_TEST_RUNTIME_PROFILE_DESIGNER = 5,
    UMI_TEST_RUNTIME_PROFILE_CONTEXT = 6,
    UMI_TEST_RUNTIME_PROFILE_EDITOR = 7,
    UMI_TEST_RUNTIME_PROFILE_STUDIO = 8,
    UMI_TEST_RUNTIME_PROFILE_INTEGRATION = 9,
    UMI_TEST_RUNTIME_PROFILE_FULL_REGRESSION = 10
} UmiTestRuntimeProfileKind;

typedef struct UmiTestRuntimeTimeBudget {
    uint64_t start_timeout_ms;
    uint64_t execution_timeout_ms;
    uint64_t shutdown_timeout_ms;
} UmiTestRuntimeTimeBudget;

typedef struct UmiTestRuntimeFailureEvidence {
    uint32_t structure_size;
    UmiTestRuntimeFailureKind kind;
    char test_id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char executable[UMI_TEST_RUNTIME_PATH_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    int64_t native_status;
    int exit_code;
    uint64_t occurred_at_ms;
    uint64_t revision;
} UmiTestRuntimeFailureEvidence;

const char *umi_test_runtime_state_text(UmiTestRuntimeState state);
const char *umi_test_runtime_failure_kind_text(UmiTestRuntimeFailureKind kind);
const char *umi_test_runtime_profile_kind_text(UmiTestRuntimeProfileKind kind);
UmiStatus umi_test_runtime_copy_text(char *destination, size_t capacity, const char *source);
bool umi_test_runtime_text_is_valid(const char *text, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
