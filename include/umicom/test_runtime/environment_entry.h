/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/environment_entry.h
 *
 * PURPOSE:
 *   Represent one inherited or overridden test environment variable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_ENVIRONMENT_ENTRY
#define UMICOM_TEST_RUNTIME_ENVIRONMENT_ENTRY
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeEnvironmentEntry {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t redacted;
    uint64_t revision;
    bool active;
} UmiTestRuntimeEnvironmentEntry;
void umi_test_runtime_environment_entry_init(UmiTestRuntimeEnvironmentEntry *value,const char *id);
UmiStatus umi_test_runtime_environment_entry_validate(const UmiTestRuntimeEnvironmentEntry *value);
UmiStatus umi_test_runtime_environment_entry_set_category(UmiTestRuntimeEnvironmentEntry *value,const char *category);
UmiStatus umi_test_runtime_environment_entry_set_detail(UmiTestRuntimeEnvironmentEntry *value,const char *detail);
UmiStatus umi_test_runtime_environment_entry_set_required(UmiTestRuntimeEnvironmentEntry *value,uint64_t number);
UmiStatus umi_test_runtime_environment_entry_set_redacted(UmiTestRuntimeEnvironmentEntry *value,uint64_t number);
UmiStatus umi_test_runtime_environment_entry_set_active(UmiTestRuntimeEnvironmentEntry *value,bool active);
bool umi_test_runtime_environment_entry_same_identity(const UmiTestRuntimeEnvironmentEntry *left,const UmiTestRuntimeEnvironmentEntry *right);
#ifdef __cplusplus
}
#endif
#endif
