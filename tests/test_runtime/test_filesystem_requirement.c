/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_filesystem_requirement.c
 *
 * PURPOSE:
 *   Verify the filesystem requirement runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/filesystem_requirement.h"
int main(void)
{
    UmiTestRuntimeFilesystemRequirement v,s;
    umi_test_runtime_filesystem_requirement_init(&v,"test-runtime.filesystem_requirement");
    assert(umi_test_runtime_filesystem_requirement_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_filesystem_requirement_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_filesystem_requirement_set_required(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_filesystem_requirement_set_available(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_filesystem_requirement_init(&s,"test-runtime.filesystem_requirement");
    assert(umi_test_runtime_filesystem_requirement_same_identity(&v,&s));
    return 0;
    }
