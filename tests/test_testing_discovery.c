/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_testing_discovery.c
 *
 * PURPOSE:
 *   Verify deterministic parsing of CTest discovery output.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/testing/discovery.h"

int main(void)
{
    UmiTestSuite *suite = NULL;
    size_t discovered = 0U;
    const char *output = "Test project /tmp/build\n"
                         "  Test #1: framework.one\n"
                         "  Test #2: framework.two\n";
    assert(umi_test_suite_create("ctest", "CTest", &suite) == UMI_STATUS_OK);
    assert(umi_test_discovery_parse_ctest(output, "/tmp/build", suite, &discovered) == UMI_STATUS_OK);
    assert(discovered == 2U);
    assert(umi_test_suite_count(suite) == 2U);
    umi_test_suite_destroy(suite);
    return 0;
}
