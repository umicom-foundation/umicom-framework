/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/test_runtime/database_requirement.h
 *
 * PURPOSE:
 *   Describe database provider and isolation requirements for a test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RUNTIME_DATABASE_REQUIREMENT
#define UMICOM_TEST_RUNTIME_DATABASE_REQUIREMENT
#include "umicom/test_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the test runtime database requirement data shared with callers of this public
 * contract.
 */
typedef struct UmiTestRuntimeDatabaseRequirement
{
    uint32_t structure_size;
    char id[UMI_TEST_RUNTIME_ID_CAPACITY];
    char detail[UMI_TEST_RUNTIME_TEXT_CAPACITY];
    uint64_t required;
    uint64_t available;
    uint64_t revision;
    bool enabled;
    } UmiTestRuntimeDatabaseRequirement;
/**
 * Initialise test runtime database requirement from caller-provided values so later
 * operations receive a known state.
 */
void umi_test_runtime_database_requirement_init(UmiTestRuntimeDatabaseRequirement *value,const char *id);
/**
 * Check that test runtime database requirement satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_test_runtime_database_requirement_validate(const UmiTestRuntimeDatabaseRequirement *value);
/**
 * Provide the test runtime database requirement set detail operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_database_requirement_set_detail(UmiTestRuntimeDatabaseRequirement *value,const char *detail);
/**
 * Provide the test runtime database requirement set required operation used by this module
 * and its client applications.
 */
UmiStatus umi_test_runtime_database_requirement_set_required(UmiTestRuntimeDatabaseRequirement *value,uint64_t number);
/**
 * Provide the test runtime database requirement set available operation used by this
 * module and its client applications.
 */
UmiStatus umi_test_runtime_database_requirement_set_available(UmiTestRuntimeDatabaseRequirement *value,uint64_t number);
/**
 * Provide the test runtime database requirement same identity operation used by this
 * module and its client applications.
 */
bool umi_test_runtime_database_requirement_same_identity(const UmiTestRuntimeDatabaseRequirement *left,const UmiTestRuntimeDatabaseRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
