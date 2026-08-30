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
void umi_test_runtime_database_requirement_init(UmiTestRuntimeDatabaseRequirement *value,const char *id);
UmiStatus umi_test_runtime_database_requirement_validate(const UmiTestRuntimeDatabaseRequirement *value);
UmiStatus umi_test_runtime_database_requirement_set_detail(UmiTestRuntimeDatabaseRequirement *value,const char *detail);
UmiStatus umi_test_runtime_database_requirement_set_required(UmiTestRuntimeDatabaseRequirement *value,uint64_t number);
UmiStatus umi_test_runtime_database_requirement_set_available(UmiTestRuntimeDatabaseRequirement *value,uint64_t number);
bool umi_test_runtime_database_requirement_same_identity(const UmiTestRuntimeDatabaseRequirement *left,const UmiTestRuntimeDatabaseRequirement *right);
#ifdef __cplusplus
}
#endif
#endif
