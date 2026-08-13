#include <assert.h>
#include <string.h>
#include "umicom/diagnostics/runtime_parser.h"
int main(void)
{
    UmiOutputRecord output;
    UmiDiagnosticSnapshot item;
    int matched = 0;
    assert(umi_output_record_init(&output, "debug", "Debug", "runtime",
        UMI_OUTPUT_STREAM_DEBUG, "AddressSanitizer: heap-use-after-free") == UMI_STATUS_OK);
    assert(umi_runtime_diagnostic_parse(&output, &item, &matched, NULL) == UMI_STATUS_OK);
    assert(matched && item.kind == UMI_DIAGNOSTIC_KIND_RUNTIME);
    assert(strcmp(item.code, "address-sanitizer") == 0);
    return 0;
}
