/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/working_directory.h
 *
 * PURPOSE:
 *   Validate and retain the working directory selected for a test process.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_WORKING_DIRECTORY
#define UMICOM_TEST_RUNTIME_WORKING_DIRECTORY

#include "umicom/test_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTestRuntimeWorkingDirectory {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char name[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t exists;
    uint64_t generation;
    uint64_t revision;
    uint64_t updated_at_ms;
    bool enabled;
} UmiTestRuntimeWorkingDirectory;

void umi_test_runtime_working_directory_init(UmiTestRuntimeWorkingDirectory *value, const char *id);
UmiStatus umi_test_runtime_working_directory_validate(const UmiTestRuntimeWorkingDirectory *value);
UmiStatus umi_test_runtime_working_directory_set_name(UmiTestRuntimeWorkingDirectory *value, const char *name);
UmiStatus umi_test_runtime_working_directory_set_detail(UmiTestRuntimeWorkingDirectory *value, const char *detail);
UmiStatus umi_test_runtime_working_directory_set_exists(UmiTestRuntimeWorkingDirectory *value, uint64_t number);
UmiStatus umi_test_runtime_working_directory_set_generation(UmiTestRuntimeWorkingDirectory *value, uint64_t number);
UmiStatus umi_test_runtime_working_directory_touch(UmiTestRuntimeWorkingDirectory *value, uint64_t updated_at_ms);
bool umi_test_runtime_working_directory_same_identity(const UmiTestRuntimeWorkingDirectory *left, const UmiTestRuntimeWorkingDirectory *right);

#ifdef __cplusplus
}
#endif
#endif
