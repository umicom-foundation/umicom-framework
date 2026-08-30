/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_profile.c
 *
 * PURPOSE:
 *   Verify the profile contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/profile.h"

int main(void)
{
    UmiTestRuntimeProfile value;
    UmiTestRuntimeProfile same;
    uint64_t revision;
    umi_test_runtime_profile_init(&value, "test-runtime.profile");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_profile_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_profile_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_profile_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_profile_set_test_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_profile_set_maximum_parallel(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_profile_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.test_count == 7U);
    assert(value.maximum_parallel == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_profile_init(&same, "test-runtime.profile");
    assert(umi_test_runtime_profile_same_identity(&value, &same));
    assert(umi_test_runtime_profile_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
