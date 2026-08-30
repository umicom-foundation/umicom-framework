/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_quarantine.c
 *
 * PURPOSE:
 *   Verify the quarantine runtime contract.
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
#include "umicom/test_runtime/quarantine.h"
int main(void)
{
    UmiTestRuntimeQuarantine value,same;
    uint64_t r;
    umi_test_runtime_quarantine_init(&value,"test-runtime.quarantine");
    assert(umi_test_runtime_quarantine_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_quarantine_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_quarantine_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_quarantine_set_expires_at_ms(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_quarantine_set_issue_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_quarantine_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.expires_at_ms==13U&&value.issue_count==21U);
    umi_test_runtime_quarantine_init(&same,"test-runtime.quarantine");
    assert(umi_test_runtime_quarantine_same_identity(&value,&same));
    return 0;
    }
