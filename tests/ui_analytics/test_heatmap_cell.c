/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_heatmap_cell.c
 *
 * PURPOSE:
 *   Validate heatmap_cell analytics behaviour.
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
#include "umicom/ui/analytics/heatmap_cell.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/heatmap_cell.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsHeatmapCellTransferEqual(const UmiAnalyticsHeatmapCell *a, const UmiAnalyticsHeatmapCell *b)
{
    return a->row == b->row &&
        a->column == b->column &&
        a->value == b->value &&
        strcmp(a->label, b->label) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsHeatmapCellTransferTails(UmiAnalyticsHeatmapCell *value)
{
    (void)value;
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsHeatmapCellTransferMalformed(const UmiAnalyticsHeatmapCell *sample)
{
    (void)sample;
    {
        UmiAnalyticsHeatmapCell invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_heatmap_cell_valid(&invalid)) ||
            umi_analytics_heatmap_cell_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsHeatmapCellTransferCases, UmiAnalyticsHeatmapCell,
    umi_analytics_heatmap_cell_archive_encode, umi_analytics_heatmap_cell_archive_decode,
    UmiAnalyticsHeatmapCellTransferEqual, UmiAnalyticsHeatmapCellTransferTails, UmiAnalyticsHeatmapCellTransferMalformed)

int main(void){UmiAnalyticsHeatmapCell item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_heatmap_cell_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsHeatmapCellTransferCases(&item) != 0) return 1;
return (umi_analytics_heatmap_cell_valid(&item))?0:2;}
