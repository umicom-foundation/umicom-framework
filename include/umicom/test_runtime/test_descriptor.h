/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/test_descriptor.h
 *
 * PURPOSE:
 *   Describe a registered test without depending on CTest internal structures.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TEST_DESCRIPTOR
#define UMICOM_TEST_RUNTIME_TEST_DESCRIPTOR

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_test_runtime_test_descriptor_init(UmiTestRuntimeTestDescriptor *value, const char *id);
UmiStatus umi_test_runtime_test_descriptor_validate(const UmiTestRuntimeTestDescriptor *value);
UmiStatus umi_test_runtime_test_descriptor_set_name(UmiTestRuntimeTestDescriptor *value, const char *name);
UmiStatus umi_test_runtime_test_descriptor_set_detail(UmiTestRuntimeTestDescriptor *value, const char *detail);
UmiStatus umi_test_runtime_test_descriptor_set_timeout_seconds(UmiTestRuntimeTestDescriptor *value, uint64_t number);
UmiStatus umi_test_runtime_test_descriptor_set_processor_cost(UmiTestRuntimeTestDescriptor *value, uint64_t number);
UmiStatus umi_test_runtime_test_descriptor_touch(UmiTestRuntimeTestDescriptor *value, uint64_t updated_at_ms);
bool umi_test_runtime_test_descriptor_same_identity(const UmiTestRuntimeTestDescriptor *left, const UmiTestRuntimeTestDescriptor *right);

#ifdef __cplusplus
}
#endif
#endif
