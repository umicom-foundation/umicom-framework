/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/boot_report_json/main.c
 *
 * PURPOSE:
 *   Create structured captured-report comparisons using installed Framework interfaces only.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/platform/boot_report_json.h"
#include <stdio.h>
#include <string.h>
int main(void)
{
    /* Programmatically constructed teaching snapshots are deliberately labelled
     * captured observations. Real clients first use UmiBootReportLoad. */
    UmiBootReport before = {0};
    before.mode = UMI_BOOT_REPORT_NORMAL; before.state = UMI_BOOT_REPORT_STARTING;
    before.planned = 2U; before.completed = 1U;
    memcpy(before.reason, "none", 5U);
    memcpy(before.sourceId, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef", 65U);
    UmiBootReport after = before;
    after.state = UMI_BOOT_REPORT_READY; after.completed = 2U;
    char json[UMI_BOOT_REPORT_JSON_CAPACITY]; size_t length = 0U;
    if (UmiBootReportComparisonJson(&before, &after, json, sizeof json, &length) != UMI_STATUS_OK) return 1;
    if (length != strlen(json) || !strstr(json, "\"changed_fields\":[\"state\",\"completed\"]")) return 1;
    if (fwrite(json, 1U, length, stdout) != length || fputc('\n', stdout) == EOF) return 1;
    return fflush(stdout) == 0 ? 0 : 1;
}
