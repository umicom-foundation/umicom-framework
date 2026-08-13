#include <assert.h>
#include <string.h>
#include "umicom/diagnostics/filter.h"
int main(void)
{
    UmiDiagnosticSnapshot item;
    UmiDiagnosticFilter filter;
    assert(umi_diagnostic_snapshot_init(&item, "f1", UMI_DIAGNOSTIC_WARNING,
        UMI_DIAGNOSTIC_KIND_LANGUAGE, "clangd", "Unknown Symbol") == UMI_STATUS_OK);
    (void)strcpy(item.uri, "src/main.c");
    umi_diagnostic_filter_init(&filter);
    (void)strcpy(filter.text, "symbol");
    assert(umi_diagnostic_filter_matches(&filter, &item));
    umi_diagnostic_filter_set_minimum_severity(&filter, UMI_DIAGNOSTIC_ERROR);
    assert(!umi_diagnostic_filter_matches(&filter, &item));
    return 0;
}
