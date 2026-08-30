/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_search_path.c
 *
 * PURPOSE:
 *   Verify the search path contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/search_path.h"

int main(void)
{
    UmiTestRuntimeSearchPath value;
    UmiTestRuntimeSearchPath same;
    uint64_t revision;
    umi_test_runtime_search_path_init(&value, "test-runtime.search_path");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_search_path_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_search_path_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_search_path_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_search_path_set_path_count(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_search_path_set_generation(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_search_path_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.path_count == 7U);
    assert(value.generation == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_search_path_init(&same, "test-runtime.search_path");
    assert(umi_test_runtime_search_path_same_identity(&value, &same));
    assert(umi_test_runtime_search_path_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
