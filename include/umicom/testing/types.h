/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/types.h
 *
 * PURPOSE:
 *   Define test identifiers, kinds, states and common capacity limits used by Framework test discovery and execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_TYPES_H
#define UMICOM_TESTING_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TEST_ID_CAPACITY 160U
#define UMI_TEST_NAME_CAPACITY 256U
#define UMI_TEST_LABEL_CAPACITY 128U
#define UMI_TEST_COMMAND_CAPACITY 2048U
#define UMI_TEST_OUTPUT_CAPACITY 32768U
#define UMI_TEST_SUITE_MAX_CASES 1024U
#define UMI_TEST_REGISTRY_MAX_SUITES 128U

typedef enum UmiTestKind {
    UMI_TEST_KIND_UNIT = 0,
    UMI_TEST_KIND_CONTRACT = 1,
    UMI_TEST_KIND_INTEGRATION = 2,
    UMI_TEST_KIND_SYSTEM = 3,
    UMI_TEST_KIND_EXTERNAL = 4
} UmiTestKind;

typedef enum UmiTestState {
    UMI_TEST_STATE_NOT_RUN = 0,
    UMI_TEST_STATE_RUNNING = 1,
    UMI_TEST_STATE_PASSED = 2,
    UMI_TEST_STATE_FAILED = 3,
    UMI_TEST_STATE_SKIPPED = 4,
    UMI_TEST_STATE_CANCELLED = 5,
    UMI_TEST_STATE_TIMED_OUT = 6
} UmiTestState;

const char *umi_test_kind_text(UmiTestKind kind);
const char *umi_test_state_text(UmiTestState state);

#ifdef __cplusplus
}
#endif

#endif
