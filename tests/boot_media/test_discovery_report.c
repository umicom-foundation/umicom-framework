/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/boot_media/test_discovery_report.c
 *
 * PURPOSE:
 *   Verify inventory diagnostics replace stale transfer evidence without querying native devices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include "umicom/boot_media/media.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static UmiStatus fixture_status;
static unsigned fixture_calls;
static int fixture_empty_detail, fixture_stale_fields;
/* Rename only the native observation seam. The public wrapper below is the
 * real source, and cannot reach the host disk scanner in this fixture. */
#define BmDiscover FixtureDiscover
#include "../../src/boot_media/discovery.c"
#undef BmDiscover

UmiStatus FixtureDiscover(UmiBootMediaInventory *out, UmiBootMediaReport *report)
{
    ++fixture_calls;
    assert(out != NULL && report != NULL);
    assert(out->count == 0U && out->omitted == 0U);
    assert(report->bytesDone == 0U && report->verified == 0);
    out->count = 1U;
    strcpy(out->devices[0].identity, "synthetic-observation");
    if (fixture_status == UMI_STATUS_CAPACITY_EXCEEDED) out->omitted = 2U;
    if (!fixture_empty_detail) strcpy(report->detail, "Synthetic adapter result");
    if (fixture_stale_fields) {
        report->status = UMI_STATUS_OK;
        report->phase = UMI_BOOT_MEDIA_FINISHED;
        report->bytesDone = 999U; report->bytesTotal = 999U;
        report->outputCreated = 1; report->writeStarted = 1; report->verified = 1;
    }
    return fixture_status;
}
static void Stale(UmiBootMediaReport *report)
{
    memset(report, 0x5a, sizeof(*report));
    report->status = UMI_STATUS_OK;
    report->phase = UMI_BOOT_MEDIA_FINISHED;
    report->outputCreated = report->writeStarted = report->verified = 1;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    int null_output = 0, null_report = 0, repeat = 0;
    fixture_status = UMI_STATUS_OK;
    if (!strcmp(mode, "success")) {}
    else if (!strcmp(mode, "unavailable")) fixture_status = UMI_STATUS_UNAVAILABLE;
    else if (!strcmp(mode, "capacity")) fixture_status = UMI_STATUS_CAPACITY_EXCEEDED;
    else if (!strcmp(mode, "permission")) fixture_status = UMI_STATUS_PERMISSION_DENIED;
    else if (!strcmp(mode, "null-output")) null_output = 1;
    else if (!strcmp(mode, "null-report")) null_report = 1;
    else if (!strcmp(mode, "empty-success")) fixture_empty_detail = 1;
    else if (!strcmp(mode, "empty-failure")) { fixture_empty_detail = 1; fixture_status = UMI_STATUS_IO_ERROR; }
    else if (!strcmp(mode, "adapter-stale")) { fixture_stale_fields = 1; fixture_status = UMI_STATUS_IO_ERROR; }
    else if (!strcmp(mode, "repeat")) repeat = 1;
    else return 2;
    UmiBootMediaInventory *inventory = malloc(sizeof(*inventory));
    assert(inventory != NULL);
    memset(inventory, 0x6b, sizeof(*inventory));
    UmiBootMediaReport report;
    Stale(&report);
    UmiStatus expected = null_output ? UMI_STATUS_INVALID_ARGUMENT : fixture_status;
    UmiStatus result = UmiBootMediaDiscover(null_output ? NULL : inventory, null_report ? NULL : &report);
    assert(result == expected);
    assert(fixture_calls == (null_output ? 0U : 1U));
    if (!null_report) {
        assert(report.status == result && report.phase == UMI_BOOT_MEDIA_INSPECT);
        assert(report.bytesDone == 0U && report.bytesTotal == 0U);
        assert(!report.outputCreated && !report.writeStarted && !report.verified);
        assert(report.detail[0] != '\0' && memchr(report.detail, 0, sizeof(report.detail)) != NULL);
    }
    if (!null_output) {
        assert(inventory->count == 1U);
        assert(!strcmp(inventory->devices[0].identity, "synthetic-observation"));
        assert(inventory->omitted == (fixture_status == UMI_STATUS_CAPACITY_EXCEEDED ? 2U : 0U));
    }
    if (repeat) {
        fixture_status = UMI_STATUS_UNAVAILABLE;
        assert(UmiBootMediaDiscover(inventory, &report) == UMI_STATUS_UNAVAILABLE);
        assert(report.status == UMI_STATUS_UNAVAILABLE && !report.verified && !report.writeStarted);
        assert(fixture_calls == 2U);
    }
    free(inventory);
    return 0;
}
