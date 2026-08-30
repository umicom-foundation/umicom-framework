/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_report.c
 *
 * PURPOSE:
 *   Verify the report contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/report.h"

int main(void)
{
    UmiTestRuntimeReport value;
    UmiTestRuntimeReport same;
    uint64_t revision;
    umi_test_runtime_report_init(&value, "test-runtime.report");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_report_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_report_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_report_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_report_set_passed(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_report_set_failed(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_report_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.passed == 7U);
    assert(value.failed == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_report_init(&same, "test-runtime.report");
    assert(umi_test_runtime_report_same_identity(&value, &same));
    assert(umi_test_runtime_report_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
