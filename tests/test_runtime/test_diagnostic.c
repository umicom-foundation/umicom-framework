/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_diagnostic.c
 *
 * PURPOSE:
 *   Verify the diagnostic contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/diagnostic.h"

int main(void)
{
    UmiTestRuntimeDiagnostic value;
    UmiTestRuntimeDiagnostic same;
    uint64_t revision;
    umi_test_runtime_diagnostic_init(&value, "test-runtime.diagnostic");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_diagnostic_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_diagnostic_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_set_severity(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_set_sequence(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_diagnostic_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.severity == 7U);
    assert(value.sequence == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_diagnostic_init(&same, "test-runtime.diagnostic");
    assert(umi_test_runtime_diagnostic_same_identity(&value, &same));
    assert(umi_test_runtime_diagnostic_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
