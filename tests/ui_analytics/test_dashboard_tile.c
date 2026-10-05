/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_tile.c
 *
 * PURPOSE:
 *   Validate dashboard_tile analytics behaviour.
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
#include "umicom/ui/analytics/dashboard_tile.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/dashboard_tile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsDashboardTileTransferEqual(const UmiAnalyticsDashboardTile *a, const UmiAnalyticsDashboardTile *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->component_id, b->component_id) == 0 &&
        a->row == b->row &&
        a->column == b->column &&
        a->row_span == b->row_span &&
        a->column_span == b->column_span;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsDashboardTileTransferTails(UmiAnalyticsDashboardTile *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsDashboardTileTransferMalformed(const UmiAnalyticsDashboardTile *sample)
{
    (void)sample;
    {
        UmiAnalyticsDashboardTile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_dashboard_tile_valid(&invalid)) ||
            umi_analytics_dashboard_tile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAnalyticsDashboardTile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_dashboard_tile_valid(&invalid)) ||
            umi_analytics_dashboard_tile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsDashboardTileTransferCases, UmiAnalyticsDashboardTile,
    umi_analytics_dashboard_tile_archive_encode, umi_analytics_dashboard_tile_archive_decode,
    UmiAnalyticsDashboardTileTransferEqual, UmiAnalyticsDashboardTileTransferTails, UmiAnalyticsDashboardTileTransferMalformed)

int main(void){UmiAnalyticsDashboardTile item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_dashboard_tile_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsDashboardTileTransferCases(&item) != 0) return 1;
return (umi_analytics_dashboard_tile_valid(&item))?0:2;}
