/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_catalogue.c
 *
 * PURPOSE:
 *   Verify the catalogue contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeCatalogue value;
    UmiTestRuntimeCatalogue same;
    uint64_t revision;
    umi_test_runtime_catalogue_init(&value, "test-runtime.catalogue");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_catalogue_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_catalogue_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_catalogue_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_catalogue_set_test_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_catalogue_set_generation(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_catalogue_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.test_count == 7U);
    assert(value.generation == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_catalogue_init(&same, "test-runtime.catalogue");
    assert(umi_test_runtime_catalogue_same_identity(&value, &same));
    assert(umi_test_runtime_catalogue_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
