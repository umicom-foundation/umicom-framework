/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/test_descriptor.h
 *
 * PURPOSE:
 *   Describe a registered test without depending on CTest internal structures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TEST_DESCRIPTOR
#define UMICOM_TEST_RUNTIME_TEST_DESCRIPTOR

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the test runtime test descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeTestDescriptor {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t timeout_seconds;
    uint64_t processor_cost;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeTestDescriptor;

/**
 * Initialise test runtime test descriptor from caller-provided values so later operations
 * receive a known state.
 */
void umi_test_runtime_test_descriptor_init(UmiTestRuntimeTestDescriptor *value, const char *id);
/**
 * Check that test runtime test descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_test_runtime_test_descriptor_validate(const UmiTestRuntimeTestDescriptor *value);
/**
 * Provide the test runtime test descriptor set name operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_descriptor_set_name(UmiTestRuntimeTestDescriptor *value, const char *name);
/**
 * Provide the test runtime test descriptor set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_test_runtime_test_descriptor_set_detail(UmiTestRuntimeTestDescriptor *value, const char *detail);
/**
 * Provide the test runtime test descriptor set timeout seconds operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_test_descriptor_set_timeout_seconds(UmiTestRuntimeTestDescriptor *value, uint64_t number);
/**
 * Provide the test runtime test descriptor set processor cost operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_test_descriptor_set_processor_cost(UmiTestRuntimeTestDescriptor *value, uint64_t number);
/**
 * Provide the test runtime test descriptor touch operation used by this module and its
 * client applications.
 */
UmiStatus umi_test_runtime_test_descriptor_touch(UmiTestRuntimeTestDescriptor *value, uint64_t updated_at_ms);
/**
 * Provide the test runtime test descriptor same identity operation used by this module and
 * its client applications.
 */
bool umi_test_runtime_test_descriptor_same_identity(const UmiTestRuntimeTestDescriptor *left, const UmiTestRuntimeTestDescriptor *right);

#ifdef __cplusplus
}
#endif
#endif
