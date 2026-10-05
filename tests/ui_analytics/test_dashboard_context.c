/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_dashboard_context.c
 *
 * PURPOSE:
 *   Validate dashboard_context analytics behaviour.
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
#include "umicom/ui/analytics/dashboard_context.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/dashboard_context.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsDashboardContextTransferEqual(const UmiAnalyticsDashboardContext *a, const UmiAnalyticsDashboardContext *b)
{
    return strcmp(a->context_group, b->context_group) == 0 &&
        strcmp(a->entity_id, b->entity_id) == 0 &&
        a->start_ns == b->start_ns &&
        a->end_ns == b->end_ns;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsDashboardContextTransferTails(UmiAnalyticsDashboardContext *value)
{
    (void)value;
    {
        size_t used = strlen(value->context_group) + 1U;
        memset(value->context_group + used, 0xa5, sizeof(value->context_group) - used);
    }
    {
        size_t used = strlen(value->entity_id) + 1U;
        memset(value->entity_id + used, 0xa5, sizeof(value->entity_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsDashboardContextTransferMalformed(const UmiAnalyticsDashboardContext *sample)
{
    (void)sample;
    {
        UmiAnalyticsDashboardContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_group, 'x', sizeof(invalid.context_group));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_dashboard_context_valid(&invalid)) ||
            umi_analytics_dashboard_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_group was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAnalyticsDashboardContext invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.entity_id, 'x', sizeof(invalid.entity_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_dashboard_context_valid(&invalid)) ||
            umi_analytics_dashboard_context_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated entity_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsDashboardContextTransferCases, UmiAnalyticsDashboardContext,
    umi_analytics_dashboard_context_archive_encode, umi_analytics_dashboard_context_archive_decode,
    UmiAnalyticsDashboardContextTransferEqual, UmiAnalyticsDashboardContextTransferTails, UmiAnalyticsDashboardContextTransferMalformed)

int main(void){UmiAnalyticsDashboardContext item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_dashboard_context_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsDashboardContextTransferCases(&item) != 0) return 1;
return (umi_analytics_dashboard_context_valid(&item))?0:2;}
