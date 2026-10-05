/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_analytics_query.c
 *
 * PURPOSE:
 *   Validate analytics_query analytics behaviour.
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
#include "umicom/ui/analytics/analytics_query.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/analytics_query.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsQueryTransferEqual(const UmiAnalyticsQuery *a, const UmiAnalyticsQuery *b)
{
    return strcmp(a->dataset_id, b->dataset_id) == 0 &&
        strcmp(a->metric_id, b->metric_id) == 0 &&
        strcmp(a->group_by, b->group_by) == 0 &&
        a->start_ns == b->start_ns &&
        a->end_ns == b->end_ns &&
        a->limit == b->limit;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsQueryTransferTails(UmiAnalyticsQuery *value)
{
    (void)value;
    {
        size_t used = strlen(value->dataset_id) + 1U;
        memset(value->dataset_id + used, 0xa5, sizeof(value->dataset_id) - used);
    }
    {
        size_t used = strlen(value->metric_id) + 1U;
        memset(value->metric_id + used, 0xa5, sizeof(value->metric_id) - used);
    }
    {
        size_t used = strlen(value->group_by) + 1U;
        memset(value->group_by + used, 0xa5, sizeof(value->group_by) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsQueryTransferMalformed(const UmiAnalyticsQuery *sample)
{
    (void)sample;
    {
        UmiAnalyticsQuery invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.dataset_id, 'x', sizeof(invalid.dataset_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_query_valid(&invalid)) ||
            umi_analytics_query_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated dataset_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAnalyticsQuery invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.metric_id, 'x', sizeof(invalid.metric_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_query_valid(&invalid)) ||
            umi_analytics_query_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated metric_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAnalyticsQuery invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_by, 'x', sizeof(invalid.group_by));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_query_valid(&invalid)) ||
            umi_analytics_query_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_by was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsQueryTransferCases, UmiAnalyticsQuery,
    umi_analytics_query_archive_encode, umi_analytics_query_archive_decode,
    UmiAnalyticsQueryTransferEqual, UmiAnalyticsQueryTransferTails, UmiAnalyticsQueryTransferMalformed)

int main(void){UmiAnalyticsQuery q;/* Apply this operation only while the related capability or state is available. */ if(umi_analytics_query_init(&q,"market","price")!=0)return 1;
    if (UmiAnalyticsQueryTransferCases(&q) != 0) return 1;
return umi_analytics_query_valid(&q)?0:2;}
