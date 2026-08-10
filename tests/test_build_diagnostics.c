/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_diagnostics.c
 *
 * PURPOSE:
 *   Verify bounded build diagnostics and severity counting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/build/diagnostic.h"

int main(void)
{
    UmiBuildDiagnosticList list;
    UmiBuildDiagnostic item = {0};
    umi_build_diagnostic_list_init(&list);
    item.severity = UMI_BUILD_DIAGNOSTIC_WARNING;
    (void)strcpy(item.file, "main.c");
    (void)strcpy(item.message, "unused value");
    assert(umi_build_diagnostic_list_add(&list, &item) == UMI_STATUS_OK);
    item.severity = UMI_BUILD_DIAGNOSTIC_ERROR;
    assert(umi_build_diagnostic_list_add(&list, &item) == UMI_STATUS_OK);
    assert(list.count == 2U);
    assert(umi_build_diagnostic_list_count_severity(&list, UMI_BUILD_DIAGNOSTIC_WARNING) == 2U);
    assert(umi_build_diagnostic_list_at(&list, 1U)->severity == UMI_BUILD_DIAGNOSTIC_ERROR);
    return 0;
}
