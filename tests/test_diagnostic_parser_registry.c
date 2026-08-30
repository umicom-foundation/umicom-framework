/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_parser_registry.c
 *
 * PURPOSE:
 *   Implement the test diagnostic parser registry behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/diagnostics/parser.h"
static UmiStatus parse_custom(const UmiOutputRecord *output,
                              UmiDiagnosticSnapshot *item,
                              int *matched,
                              void *data)
{
    (void)output; (void)data;
    *matched = 1;
    return umi_diagnostic_snapshot_init(item, "custom", UMI_DIAGNOSTIC_INFO,
        UMI_DIAGNOSTIC_KIND_GENERAL, "custom", "matched");
}
int main(void)
{
    UmiDiagnosticParserRegistry *registry = NULL;
    UmiDiagnosticParser parser = {"custom", 10, parse_custom, NULL};
    UmiOutputRecord output;
    UmiDiagnosticSnapshot item;
    int matched = 0;
    assert(umi_diagnostic_parser_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_diagnostic_parser_registry_add(registry, &parser) == UMI_STATUS_OK);
    assert(umi_output_record_init(&output, "x", "X", "x",
        UMI_OUTPUT_STREAM_STANDARD, "x") == UMI_STATUS_OK);
    assert(umi_diagnostic_parser_registry_parse(registry, &output, &item, &matched) == UMI_STATUS_OK);
    assert(matched && umi_diagnostic_parser_registry_count(registry) == 1U);
    assert(umi_diagnostic_parser_registry_remove(registry, "custom") == UMI_STATUS_OK);
    umi_diagnostic_parser_registry_destroy(registry);
    return 0;
}
