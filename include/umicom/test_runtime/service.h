/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/service.h
 *
 * PURPOSE:
 *   Own test-runtime catalogues, profiles, execution evidence and health state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_SERVICE
#define UMICOM_TEST_RUNTIME_SERVICE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeService {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t state;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeService;

void umi_test_runtime_service_init(UmiTestRuntimeService *value, const char *id);
UmiStatus umi_test_runtime_service_validate(const UmiTestRuntimeService *value);
UmiStatus umi_test_runtime_service_set_name(UmiTestRuntimeService *value, const char *name);
UmiStatus umi_test_runtime_service_set_detail(UmiTestRuntimeService *value, const char *detail);
UmiStatus umi_test_runtime_service_set_state(UmiTestRuntimeService *value, uint64_t number);
UmiStatus umi_test_runtime_service_set_generation(UmiTestRuntimeService *value, uint64_t number);
UmiStatus umi_test_runtime_service_touch(UmiTestRuntimeService *value, uint64_t updated_at_ms);
bool umi_test_runtime_service_same_identity(const UmiTestRuntimeService *left, const UmiTestRuntimeService *right);

#ifdef __cplusplus
}
#endif
#endif
