/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/tests/test_diagnostic_hub.c
 *
 * PURPOSE:
 *   Implement the test diagnostic hub behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_hub.c
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/hub.h"

#include <assert.h>

static void count_sink(const UmiDiagnostic *diagnostic, void *user_data)
{
    size_t *count = (size_t *)user_data;
    assert(diagnostic != NULL);
    ++(*count);
}

int main(void)
{
    UmiDiagnosticHub hub;
    UmiDiagnostic diagnostic = {UMI_DIAGNOSTIC_INFO, "test", "hello", 42U};
    size_t first = 0U;
    size_t second = 0U;

    umi_diagnostic_hub_init(&hub);
    assert(umi_diagnostic_hub_count(&hub) == 0U);
    assert(umi_diagnostic_hub_add(&hub, count_sink, &first) == UMI_STATUS_OK);
    assert(umi_diagnostic_hub_add(&hub, count_sink, &second) == UMI_STATUS_OK);
    assert(umi_diagnostic_hub_add(&hub, count_sink, &first) == UMI_STATUS_ALREADY_EXISTS);
    assert(umi_diagnostic_hub_count(&hub) == 2U);

    umi_diagnostic_hub_emit(&diagnostic, &hub);
    assert(first == 1U);
    assert(second == 1U);

    assert(umi_diagnostic_hub_remove(&hub, count_sink, &first) == UMI_STATUS_OK);
    assert(umi_diagnostic_hub_count(&hub) == 1U);
    umi_diagnostic_hub_emit(&diagnostic, &hub);
    assert(first == 1U);
    assert(second == 2U);
    return 0;
}
