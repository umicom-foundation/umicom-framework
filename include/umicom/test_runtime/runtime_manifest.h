/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/runtime_manifest.h
 *
 * PURPOSE:
 *   Describe the expected runtime files for one test executable.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_RUNTIME_MANIFEST
#define UMICOM_TEST_RUNTIME_RUNTIME_MANIFEST
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeRuntimeManifest {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t file_count;
    uint64_t generation;
    uint64_t revision;
    bool active;
} UmiTestRuntimeRuntimeManifest;
void umi_test_runtime_runtime_manifest_init(UmiTestRuntimeRuntimeManifest *value,const char *id);
UmiStatus umi_test_runtime_runtime_manifest_validate(const UmiTestRuntimeRuntimeManifest *value);
UmiStatus umi_test_runtime_runtime_manifest_set_category(UmiTestRuntimeRuntimeManifest *value,const char *category);
UmiStatus umi_test_runtime_runtime_manifest_set_detail(UmiTestRuntimeRuntimeManifest *value,const char *detail);
UmiStatus umi_test_runtime_runtime_manifest_set_file_count(UmiTestRuntimeRuntimeManifest *value,uint64_t number);
UmiStatus umi_test_runtime_runtime_manifest_set_generation(UmiTestRuntimeRuntimeManifest *value,uint64_t number);
UmiStatus umi_test_runtime_runtime_manifest_set_active(UmiTestRuntimeRuntimeManifest *value,bool active);
bool umi_test_runtime_runtime_manifest_same_identity(const UmiTestRuntimeRuntimeManifest *left,const UmiTestRuntimeRuntimeManifest *right);
#ifdef __cplusplus
}
#endif
#endif
