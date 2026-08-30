/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/clean_machine.h
 *
 * PURPOSE:
 *   Evaluate whether installed runtime state is sufficient outside a source tree.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_CLEAN_MACHINE
#define UMICOM_TEST_RUNTIME_CLEAN_MACHINE

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeCleanMachine {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t requirement_count;
    uint64_t missing_count;
    uint64_t updated_at_ms;
    uint64_t revision;
    bool enabled;
} UmiTestRuntimeCleanMachine;

void umi_test_runtime_clean_machine_init(UmiTestRuntimeCleanMachine *value, const char *id);
UmiStatus umi_test_runtime_clean_machine_validate(const UmiTestRuntimeCleanMachine *value);
UmiStatus umi_test_runtime_clean_machine_set_name(UmiTestRuntimeCleanMachine *value, const char *name);
UmiStatus umi_test_runtime_clean_machine_set_detail(UmiTestRuntimeCleanMachine *value, const char *detail);
UmiStatus umi_test_runtime_clean_machine_set_requirement_count(UmiTestRuntimeCleanMachine *value, uint64_t number);
UmiStatus umi_test_runtime_clean_machine_set_missing_count(UmiTestRuntimeCleanMachine *value, uint64_t number);
UmiStatus umi_test_runtime_clean_machine_touch(UmiTestRuntimeCleanMachine *value, uint64_t updated_at_ms);
bool umi_test_runtime_clean_machine_same_identity(const UmiTestRuntimeCleanMachine *left, const UmiTestRuntimeCleanMachine *right);

#ifdef __cplusplus
}
#endif
#endif
