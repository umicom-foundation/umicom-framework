/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/ctest_bridge.h
 *
 * PURPOSE:
 *   Translate CTest registration and execution evidence into stable Framework records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CTEST_BRIDGE
#define UMICOM_TEST_RUNTIME_CTEST_BRIDGE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeCtestBridge {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t registered_count;
    uint64_t executed_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeCtestBridge;

void umi_test_runtime_ctest_bridge_init(UmiTestRuntimeCtestBridge *value, const char *id);
UmiStatus umi_test_runtime_ctest_bridge_validate(const UmiTestRuntimeCtestBridge *value);
UmiStatus umi_test_runtime_ctest_bridge_set_name(UmiTestRuntimeCtestBridge *value, const char *name);
UmiStatus umi_test_runtime_ctest_bridge_set_detail(UmiTestRuntimeCtestBridge *value, const char *detail);
UmiStatus umi_test_runtime_ctest_bridge_set_registered_count(UmiTestRuntimeCtestBridge *value, uint64_t number);
UmiStatus umi_test_runtime_ctest_bridge_set_executed_count(UmiTestRuntimeCtestBridge *value, uint64_t number);
UmiStatus umi_test_runtime_ctest_bridge_touch(UmiTestRuntimeCtestBridge *value, uint64_t updated_at_ms);
bool umi_test_runtime_ctest_bridge_same_identity(const UmiTestRuntimeCtestBridge *left, const UmiTestRuntimeCtestBridge *right);

#ifdef __cplusplus
}
#endif
#endif
