/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_snapshot.c
 *
 * PURPOSE:
 *   Validate dashboard_snapshot analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/analytics/dashboard_snapshot.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/dashboard_snapshot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsDashboardSnapshotTransferEqual(const UmiAnalyticsDashboardSnapshot *a, const UmiAnalyticsDashboardSnapshot *b)
{
    return strcmp(a->dashboard_id, b->dashboard_id) == 0 &&
        a->revision == b->revision &&
        a->generated_at_ns == b->generated_at_ns &&
        a->metric_count == b->metric_count &&
        a->healthy == b->healthy;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsDashboardSnapshotTransferTails(UmiAnalyticsDashboardSnapshot *value)
{
    (void)value;
    {
        size_t used = strlen(value->dashboard_id) + 1U;
        memset(value->dashboard_id + used, 0xa5, sizeof(value->dashboard_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsDashboardSnapshotTransferMalformed(const UmiAnalyticsDashboardSnapshot *sample)
{
    (void)sample;
    {
        UmiAnalyticsDashboardSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.dashboard_id, 'x', sizeof(invalid.dashboard_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_dashboard_snapshot_valid(&invalid)) ||
            umi_analytics_dashboard_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated dashboard_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsDashboardSnapshotTransferCases, UmiAnalyticsDashboardSnapshot,
    umi_analytics_dashboard_snapshot_archive_encode, umi_analytics_dashboard_snapshot_archive_decode,
    UmiAnalyticsDashboardSnapshotTransferEqual, UmiAnalyticsDashboardSnapshotTransferTails, UmiAnalyticsDashboardSnapshotTransferMalformed)

int main(void){UmiAnalyticsDashboardSnapshot item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_dashboard_snapshot_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsDashboardSnapshotTransferCases(&item) != 0) return 1;
return (umi_analytics_dashboard_snapshot_valid(&item))?0:2;}
