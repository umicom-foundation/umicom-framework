/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_database_requirement.c
 *
 * PURPOSE:
 *   Verify the database requirement runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/database_requirement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeDatabaseRequirement v,s;
    umi_test_runtime_database_requirement_init(&v,"test-runtime.database_requirement");
    assert(umi_test_runtime_database_requirement_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_database_requirement_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_database_requirement_set_required(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_database_requirement_set_available(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_database_requirement_init(&s,"test-runtime.database_requirement");
    assert(umi_test_runtime_database_requirement_same_identity(&v,&s));
    return 0;
    }
