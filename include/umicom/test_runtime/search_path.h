/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/search_path.h
 *
 * PURPOSE:
 *   Plan deterministic executable and dynamic-library search paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SEARCH_PATH
#define UMICOM_TEST_RUNTIME_SEARCH_PATH

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeSearchPath {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t path_count;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeSearchPath;

void umi_test_runtime_search_path_init(UmiTestRuntimeSearchPath *value, const char *id);
UmiStatus umi_test_runtime_search_path_validate(const UmiTestRuntimeSearchPath *value);
UmiStatus umi_test_runtime_search_path_set_name(UmiTestRuntimeSearchPath *value, const char *name);
UmiStatus umi_test_runtime_search_path_set_detail(UmiTestRuntimeSearchPath *value, const char *detail);
UmiStatus umi_test_runtime_search_path_set_path_count(UmiTestRuntimeSearchPath *value, uint64_t number);
UmiStatus umi_test_runtime_search_path_set_generation(UmiTestRuntimeSearchPath *value, uint64_t number);
UmiStatus umi_test_runtime_search_path_touch(UmiTestRuntimeSearchPath *value, uint64_t updated_at_ms);
bool umi_test_runtime_search_path_same_identity(const UmiTestRuntimeSearchPath *left, const UmiTestRuntimeSearchPath *right);

#ifdef __cplusplus
}
#endif
#endif
