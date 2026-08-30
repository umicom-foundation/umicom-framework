/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_platform/command.h
 *
 * PURPOSE:
 *   Publish stable command identities for professional test discovery,
 *   execution, reruns, coverage, benchmarks and history. Applications decide
 *   placement only; command meaning remains Framework-owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_PLATFORM_COMMAND_H
#define UMICOM_TEST_PLATFORM_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_PLATFORM_COMMAND_API_VERSION 1U

typedef enum UmiTestPlatformCommandKind {
    UMI_TEST_COMMAND_OPEN_EXPLORER = 1,
    UMI_TEST_COMMAND_REFRESH = 2,
    UMI_TEST_COMMAND_DISCOVER = 3,
    UMI_TEST_COMMAND_RUN_ALL = 4,
    UMI_TEST_COMMAND_RUN_SELECTED = 5,
    UMI_TEST_COMMAND_DEBUG_SELECTED = 6,
    UMI_TEST_COMMAND_COVERAGE_ALL = 7,
    UMI_TEST_COMMAND_COVERAGE_SELECTED = 8,
    UMI_TEST_COMMAND_RERUN_FAILED = 9,
    UMI_TEST_COMMAND_RERUN_LAST = 10,
    UMI_TEST_COMMAND_RERUN_FLAKY = 11,
    UMI_TEST_COMMAND_REPEAT_SELECTED = 12,
    UMI_TEST_COMMAND_STOP = 13,
    UMI_TEST_COMMAND_SELECT_PROFILE = 14,
    UMI_TEST_COMMAND_FILTER = 15,
    UMI_TEST_COMMAND_FILTER_FAILED = 16,
    UMI_TEST_COMMAND_FILTER_NOT_RUN = 17,
    UMI_TEST_COMMAND_CLEAR_FILTER = 18,
    UMI_TEST_COMMAND_GROUP_HIERARCHY = 19,
    UMI_TEST_COMMAND_GROUP_SUITE = 20,
    UMI_TEST_COMMAND_GROUP_FRAMEWORK = 21,
    UMI_TEST_COMMAND_GROUP_OUTCOME = 22,
    UMI_TEST_COMMAND_SORT_NAME = 23,
    UMI_TEST_COMMAND_SORT_DURATION = 24,
    UMI_TEST_COMMAND_SORT_OUTCOME = 25,
    UMI_TEST_COMMAND_EXPAND_ALL = 26,
    UMI_TEST_COMMAND_COLLAPSE_ALL = 27,
    UMI_TEST_COMMAND_NEXT_FAILURE = 28,
    UMI_TEST_COMMAND_PREVIOUS_FAILURE = 29,
    UMI_TEST_COMMAND_OPEN_SOURCE = 30,
    UMI_TEST_COMMAND_OPEN_OUTPUT = 31,
    UMI_TEST_COMMAND_OPEN_COVERAGE = 32,
    UMI_TEST_COMMAND_OPEN_BENCHMARKS = 33,
    UMI_TEST_COMMAND_OPEN_HISTORY = 34,
    UMI_TEST_COMMAND_OPEN_RERUN_PLANNER = 35,
    UMI_TEST_COMMAND_CLEAR_RESULTS = 36,
    UMI_TEST_COMMAND_CLEAR_OUTPUT = 37,
    UMI_TEST_COMMAND_CLEAR_COVERAGE = 38,
    UMI_TEST_COMMAND_COPY_RESULT = 39,
    UMI_TEST_COMMAND_EXPORT_REPORT = 40
} UmiTestPlatformCommandKind;

typedef struct UmiTestPlatformCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiTestPlatformCommandKind kind;
    const char *id;
    const char *label;
    const char *default_keybinding;
    int requires_tests;
    int requires_selection;
    int requires_results;
    int requires_active_run;
    int requires_inactive_run;
    int mutates_workspace;
} UmiTestPlatformCommandDescriptor;

size_t umi_test_platform_command_count(void);
const UmiTestPlatformCommandDescriptor *umi_test_platform_command_at(
    size_t position);
const UmiTestPlatformCommandDescriptor *umi_test_platform_command_find(
    const char *command_id);
const UmiTestPlatformCommandDescriptor *umi_test_platform_command_for_kind(
    UmiTestPlatformCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_TEST_PLATFORM_COMMAND_H */
