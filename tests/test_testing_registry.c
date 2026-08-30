/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_testing_registry.c
 *
 * PURPOSE:
 *   Verify owned test-suite registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/testing/registry.h"

int main(void)
{
    UmiTestRegistry *registry = NULL;
    UmiTestSuite *suite = NULL;
    UmiTestCase test_case;
    assert(umi_test_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_test_suite_create("framework", "Framework", &suite) == UMI_STATUS_OK);
    umi_test_case_init(&test_case, "framework.sample", "Sample");
    assert(umi_test_case_set_command(&test_case, "sample", "", ".") == UMI_STATUS_OK);
    assert(umi_test_suite_add(suite, &test_case) == UMI_STATUS_OK);
    assert(umi_test_registry_add(registry, suite) == UMI_STATUS_OK);
    assert(umi_test_registry_count(registry) == 1U);
    assert(umi_test_registry_find(registry, "framework") == suite);
    assert(umi_test_suite_find(suite, "framework.sample") != NULL);
    umi_test_registry_destroy(registry);
    return 0;
}
