/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_profile_catalogue.c
 *
 * PURPOSE:
 *   Verify the profile catalogue contract, bounded text and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/profile_catalogue.h"

int main(void)
{
    UmiTestRuntimeProfileCatalogue value;
    UmiTestRuntimeProfileCatalogue same;
    uint64_t revision;
    umi_test_runtime_profile_catalogue_init(&value, "test-runtime.profile_catalogue");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_profile_catalogue_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_profile_catalogue_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_profile_catalogue_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_profile_catalogue_set_profile_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_profile_catalogue_set_generation(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_profile_catalogue_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.profile_count == 7U);
    assert(value.generation == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_profile_catalogue_init(&same, "test-runtime.profile_catalogue");
    assert(umi_test_runtime_profile_catalogue_same_identity(&value, &same));
    assert(umi_test_runtime_profile_catalogue_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
