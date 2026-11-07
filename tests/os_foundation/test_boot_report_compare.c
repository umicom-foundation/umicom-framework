/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/os_foundation/test_boot_report_compare.c
 *
 * PURPOSE:
 *   Exercise semantic report comparison and reject malformed caller-owned snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/boot_report.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static UmiBootReport Good(void)
{
    UmiBootReport report = {0};
    report.mode = UMI_BOOT_REPORT_NORMAL;
    report.state = UMI_BOOT_REPORT_READY;
    report.planned = report.completed = 2U;
    strcpy(report.reason, "none");
    strcpy(report.sourceId, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef");
    return report;
}
static int Refused(const UmiBootReport *bad)
{
    UmiBootReport good = Good();
    uint32_t mask = UINT32_C(0xa5a5);
    CHECK(UmiBootReportValidate(bad) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBootReportCompare(&good, bad, &mask) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(mask == UINT32_C(0xa5a5));
    CHECK(UmiBootReportCompare(bad, &good, &mask) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(mask == UINT32_C(0xa5a5));
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    UmiBootReport before = Good(), after = Good();
    uint32_t mask = UINT32_C(0xa5a5);
    const char *name = argv[1];
    if (!strcmp(name, "equal")) {
        CHECK(UmiBootReportCompare(&before, &after, &mask) == UMI_STATUS_OK && mask == 0U);
        CHECK(UmiBootReportCompare(&before, &before, &mask) == UMI_STATUS_OK && mask == 0U);
    } else if (!strcmp(name, "semantic")) {
        memset(after.reason + 5, 'x', sizeof after.reason - 5U);
        CHECK(UmiBootReportCompare(&before, &after, &mask) == UMI_STATUS_OK && mask == 0U);
    } else if (!strcmp(name, "state")) {
        after.state = UMI_BOOT_REPORT_STARTING;
        CHECK(UmiBootReportCompare(&before, &after, &mask) == UMI_STATUS_OK);
        CHECK(mask == UMI_BOOT_REPORT_CHANGED_STATE);
    } else if (!strcmp(name, "counts")) {
        before.state = after.state = UMI_BOOT_REPORT_STARTING;
        after.completed = 1U;
        CHECK(UmiBootReportCompare(&before, &after, &mask) == UMI_STATUS_OK);
        CHECK(mask == UMI_BOOT_REPORT_CHANGED_COMPLETED);
        after.completed = 2U; after.planned = 3U;
        CHECK(UmiBootReportCompare(&before, &after, &mask) == UMI_STATUS_OK);
        CHECK(mask == UMI_BOOT_REPORT_CHANGED_PLANNED);
    } else if (!strcmp(name, "recovery")) {
        before.state = after.state = UMI_BOOT_REPORT_RECOVERY;
        before.completed = after.completed = 0U;
        strcpy(before.reason, "configuration"); strcpy(after.reason, "requested");
        after.mode = UMI_BOOT_REPORT_REQUESTED_RECOVERY;
        CHECK(UmiBootReportCompare(&before, &after, &mask) == UMI_STATUS_OK);
        CHECK(mask == (UMI_BOOT_REPORT_CHANGED_MODE | UMI_BOOT_REPORT_CHANGED_REASON));
    } else if (!strcmp(name, "source")) {
        after.sourceId[0] = '1';
        CHECK(UmiBootReportCompare(&before, &after, &mask) == UMI_STATUS_OK);
        CHECK(mask == UMI_BOOT_REPORT_CHANGED_SOURCE);
    } else if (!strcmp(name, "invalid-enums")) {
        after.mode = (UmiBootReportMode)99; CHECK(Refused(&after) == 0);
        after = Good(); after.state = (UmiBootReportState)99; CHECK(Refused(&after) == 0);
    } else if (!strcmp(name, "invalid-strings")) {
        memset(after.reason, 'x', sizeof after.reason); CHECK(Refused(&after) == 0);
        after = Good(); after.sourceId[64] = 'x'; CHECK(Refused(&after) == 0);
        after = Good(); after.sourceId[0] = 'G'; CHECK(Refused(&after) == 0);
        after = Good(); after.sourceId[1] = 0; CHECK(Refused(&after) == 0);
        after = Good(); strcpy(after.reason, "unrecognised"); CHECK(Refused(&after) == 0);
    } else if (!strcmp(name, "invalid-counts")) {
        after.planned = after.completed = 17U; CHECK(Refused(&after) == 0);
        after = Good(); after.completed = 3U; CHECK(Refused(&after) == 0);
        after = Good(); after.planned = after.completed = 0U; CHECK(Refused(&after) == 0);
    } else if (!strcmp(name, "invalid-state")) {
        after.completed = 1U; CHECK(Refused(&after) == 0);
        after = Good(); after.mode = UMI_BOOT_REPORT_REQUESTED_RECOVERY; CHECK(Refused(&after) == 0);
        after = Good(); after.state = UMI_BOOT_REPORT_RECOVERY; CHECK(Refused(&after) == 0);
        strcpy(after.reason, "requested"); CHECK(Refused(&after) == 0);
        after.mode = UMI_BOOT_REPORT_REQUESTED_RECOVERY; CHECK(Refused(&after) == 0);
        after.completed = 0U; CHECK(UmiBootReportValidate(&after) == UMI_STATUS_OK);
    } else if (!strcmp(name, "arguments")) {
        CHECK(Refused(NULL) == 0);
        CHECK(UmiBootReportCompare(&before, &after, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else return 2;
    return 0;
}
