/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_reference_line.c
 *
 * PURPOSE:
 *   Validate reference_line analytics behaviour.
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
#include "umicom/ui/analytics/reference_line.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/reference_line.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsReferenceLineTransferEqual(const UmiAnalyticsReferenceLine *a, const UmiAnalyticsReferenceLine *b)
{
    return strcmp(a->label, b->label) == 0 &&
        a->orientation == b->orientation &&
        a->value == b->value;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsReferenceLineTransferTails(UmiAnalyticsReferenceLine *value)
{
    (void)value;
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsReferenceLineTransferMalformed(const UmiAnalyticsReferenceLine *sample)
{
    (void)sample;
    {
        UmiAnalyticsReferenceLine invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_analytics_reference_line_valid(&invalid)) ||
            umi_analytics_reference_line_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsReferenceLineTransferCases, UmiAnalyticsReferenceLine,
    umi_analytics_reference_line_archive_encode, umi_analytics_reference_line_archive_decode,
    UmiAnalyticsReferenceLineTransferEqual, UmiAnalyticsReferenceLineTransferTails, UmiAnalyticsReferenceLineTransferMalformed)

int main(void){UmiAnalyticsReferenceLine item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_reference_line_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsReferenceLineTransferCases(&item) != 0) return 1;
return (umi_analytics_reference_line_valid(&item))?0:2;}
