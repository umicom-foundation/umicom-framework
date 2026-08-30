/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_diagnostic_bundle.c
 *
 * PURPOSE:
 *   Verify the diagnostic bundle runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/diagnostic_bundle.h"
int main(void)
{
    UmiTestRuntimeDiagnosticBundle value,same;
    uint64_t r;
    umi_test_runtime_diagnostic_bundle_init(&value,"test-runtime.diagnostic_bundle");
    assert(umi_test_runtime_diagnostic_bundle_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_diagnostic_bundle_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_bundle_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_bundle_set_artifact_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_bundle_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_bundle_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.artifact_count==13U&&value.generation==21U);
    umi_test_runtime_diagnostic_bundle_init(&same,"test-runtime.diagnostic_bundle");
    assert(umi_test_runtime_diagnostic_bundle_same_identity(&value,&same));
    return 0;
    }
