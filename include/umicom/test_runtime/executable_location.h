/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/executable_location.h
 *
 * PURPOSE:
 *   Represent one candidate test executable location and selection evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_EXECUTABLE_LOCATION
#define UMICOM_TEST_RUNTIME_EXECUTABLE_LOCATION
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime executable location data shared with callers of this public
 * contract.
 */
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
/**
 * Initialise test runtime executable location from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_executable_location_init(UmiTestRuntimeExecutableLocation *value,const char *id);
/**
 * Check that test runtime executable location satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_executable_location_validate(const UmiTestRuntimeExecutableLocation *value);
/**
 * Provide the test runtime executable location set category operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_category(UmiTestRuntimeExecutableLocation *value,const char *category);
/**
 * Provide the test runtime executable location set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_detail(UmiTestRuntimeExecutableLocation *value,const char *detail);
/**
 * Provide the test runtime executable location set priority operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_priority(UmiTestRuntimeExecutableLocation *value,uint64_t number);
/**
 * Provide the test runtime executable location set exists operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_exists(UmiTestRuntimeExecutableLocation *value,uint64_t number);
/**
 * Provide the test runtime executable location set active operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_executable_location_set_active(UmiTestRuntimeExecutableLocation *value,bool active);
/**
 * Provide the test runtime executable location same identity operation used by this module
 * and its client applications.
 */
bool umi_test_runtime_executable_location_same_identity(const UmiTestRuntimeExecutableLocation *left,const UmiTestRuntimeExecutableLocation *right);
#ifdef __cplusplus
}
#endif
#endif
