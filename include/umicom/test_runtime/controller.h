/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/controller.h
 *
 * PURPOSE:
 *   Implement the Test Runtime Slave Controller lifecycle and command boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CONTROLLER
#define UMICOM_TEST_RUNTIME_CONTROLLER

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime controller data shared with callers of this public contract.
 */
typedef struct UmiTestRuntimeController {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t state;
    uint64_t command_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeController;

/**
 * Initialise test runtime controller from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_controller_init(UmiTestRuntimeController *value, const char *id);
/**
 * Check that test runtime controller satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_test_runtime_controller_validate(const UmiTestRuntimeController *value);
/**
 * Provide the test runtime controller set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_controller_set_name(UmiTestRuntimeController *value, const char *name);
/**
 * Provide the test runtime controller set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_controller_set_detail(UmiTestRuntimeController *value, const char *detail);
/**
 * Provide the test runtime controller set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_controller_set_state(UmiTestRuntimeController *value, uint64_t number);
/**
 * Return the number of records represented by test runtime controller set command without
 * changing their state.
 */
UmiStatus umi_test_runtime_controller_set_command_count(UmiTestRuntimeController *value, uint64_t number);
/**
 * Provide the test runtime controller touch operation used by this module and its client
 * applications.
 */
UmiStatus umi_test_runtime_controller_touch(UmiTestRuntimeController *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime controller same identity operation used by this module and its
 * client applications.
 */
bool umi_test_runtime_controller_same_identity(const UmiTestRuntimeController *left, const UmiTestRuntimeController *right);

#ifdef __cplusplus
}
#endif
#endif
