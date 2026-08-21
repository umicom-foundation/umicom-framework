/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/temporary_directory.h
 *
 * PURPOSE:
 *   Describe isolated temporary state used by one test execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_TEMPORARY_DIRECTORY
#define UMICOM_TEST_RUNTIME_TEMPORARY_DIRECTORY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeTemporaryDirectory {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t created;
    uint64_t cleanup_required;
    uint64_t revision;
    bool active;
} UmiTestRuntimeTemporaryDirectory;
void umi_test_runtime_temporary_directory_init(UmiTestRuntimeTemporaryDirectory *value,const char *id);
UmiStatus umi_test_runtime_temporary_directory_validate(const UmiTestRuntimeTemporaryDirectory *value);
UmiStatus umi_test_runtime_temporary_directory_set_category(UmiTestRuntimeTemporaryDirectory *value,const char *category);
UmiStatus umi_test_runtime_temporary_directory_set_detail(UmiTestRuntimeTemporaryDirectory *value,const char *detail);
UmiStatus umi_test_runtime_temporary_directory_set_created(UmiTestRuntimeTemporaryDirectory *value,uint64_t number);
UmiStatus umi_test_runtime_temporary_directory_set_cleanup_required(UmiTestRuntimeTemporaryDirectory *value,uint64_t number);
UmiStatus umi_test_runtime_temporary_directory_set_active(UmiTestRuntimeTemporaryDirectory *value,bool active);
bool umi_test_runtime_temporary_directory_same_identity(const UmiTestRuntimeTemporaryDirectory *left,const UmiTestRuntimeTemporaryDirectory *right);
#ifdef __cplusplus
}
#endif
#endif
