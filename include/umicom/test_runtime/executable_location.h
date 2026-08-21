/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/executable_location.h
 *
 * PURPOSE:
 *   Represent one candidate test executable location and selection evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTABLE_LOCATION
#define UMICOM_TEST_RUNTIME_EXECUTABLE_LOCATION
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTestRuntimeExecutableLocation {
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char category[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t priority;
    uint64_t exists;
    uint64_t revision;
    bool active;
} UmiTestRuntimeExecutableLocation;
void umi_test_runtime_executable_location_init(UmiTestRuntimeExecutableLocation *value,const char *id);
UmiStatus umi_test_runtime_executable_location_validate(const UmiTestRuntimeExecutableLocation *value);
UmiStatus umi_test_runtime_executable_location_set_category(UmiTestRuntimeExecutableLocation *value,const char *category);
UmiStatus umi_test_runtime_executable_location_set_detail(UmiTestRuntimeExecutableLocation *value,const char *detail);
UmiStatus umi_test_runtime_executable_location_set_priority(UmiTestRuntimeExecutableLocation *value,uint64_t number);
UmiStatus umi_test_runtime_executable_location_set_exists(UmiTestRuntimeExecutableLocation *value,uint64_t number);
UmiStatus umi_test_runtime_executable_location_set_active(UmiTestRuntimeExecutableLocation *value,bool active);
bool umi_test_runtime_executable_location_same_identity(const UmiTestRuntimeExecutableLocation *left,const UmiTestRuntimeExecutableLocation *right);
#ifdef __cplusplus
}
#endif
#endif
