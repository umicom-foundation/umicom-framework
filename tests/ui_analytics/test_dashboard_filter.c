/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_filter.c
 *
 * PURPOSE:
 *   Validate dashboard_filter analytics behaviour.
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
#include "umicom/ui/analytics/dashboard_filter.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/dashboard_filter.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsDashboardFilterTransferEqual(const UmiAnalyticsDashboardFilter *a, const UmiAnalyticsDashboardFilter *b)
{
    return strcmp(a->key, b->key) == 0 &&
        strcmp(a->value, b->value) == 0 &&
        a->case_sensitive == b->case_sensitive;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsDashboardFilterTransferTails(UmiAnalyticsDashboardFilter *value)
{
    (void)value;
    {
        size_t used = strlen(value->key) + 1U;
        memset(value->key + used, 0xa5, sizeof(value->key) - used);
    }
    {
        size_t used = strlen(value->value) + 1U;
        memset(value->value + used, 0xa5, sizeof(value->value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsDashboardFilterTransferMalformed(const UmiAnalyticsDashboardFilter *sample)
{
    (void)sample;
    {
        UmiAnalyticsDashboardFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.key, 'x', sizeof(invalid.key));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_dashboard_filter_valid(&invalid)) ||
            umi_analytics_dashboard_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated key was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAnalyticsDashboardFilter invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.value, 'x', sizeof(invalid.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_dashboard_filter_valid(&invalid)) ||
            umi_analytics_dashboard_filter_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsDashboardFilterTransferCases, UmiAnalyticsDashboardFilter,
    umi_analytics_dashboard_filter_archive_encode, umi_analytics_dashboard_filter_archive_decode,
    UmiAnalyticsDashboardFilterTransferEqual, UmiAnalyticsDashboardFilterTransferTails, UmiAnalyticsDashboardFilterTransferMalformed)

int main(void){UmiAnalyticsDashboardFilter item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_dashboard_filter_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsDashboardFilterTransferCases(&item) != 0) return 1;
return (umi_analytics_dashboard_filter_valid(&item))?0:2;}
