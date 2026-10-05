/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_customer_segment.c
 *
 * PURPOSE:
 *   Exercise customer segment validation and calculations.
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
#include "umicom/finance/banking/customer_segment.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/banking/customer_segment.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiBankingCustomerSegmentTransferEqual(const UmiBankingCustomerSegment *a, const UmiBankingCustomerSegment *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->customer_id.value, b->customer_id.value) == 0 &&
        a->segment == b->segment;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiBankingCustomerSegmentTransferTails(UmiBankingCustomerSegment *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->customer_id.value) + 1U;
        memset(value->customer_id.value + used, 0xa5, sizeof(value->customer_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiBankingCustomerSegmentTransferMalformed(const UmiBankingCustomerSegment *sample)
{
    (void)sample;
    {
        UmiBankingCustomerSegment invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_customer_segment_valid(&invalid)) ||
            umi_banking_customer_segment_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBankingCustomerSegment invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.customer_id.value, 'x', sizeof(invalid.customer_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_customer_segment_valid(&invalid)) ||
            umi_banking_customer_segment_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated customer_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiBankingCustomerSegmentTransferCases, UmiBankingCustomerSegment,
    umi_banking_customer_segment_archive_encode, umi_banking_customer_segment_archive_decode,
    UmiBankingCustomerSegmentTransferEqual, UmiBankingCustomerSegmentTransferTails, UmiBankingCustomerSegmentTransferMalformed)

int main(void) {
    UmiBankingCustomerSegment v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_customer_segment_init(&v, "seg-1", "cust-1", UMI_BANKING_SEGMENT_CORPORATE)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_customer_segment_valid(&v)) return 2;
    if (UmiBankingCustomerSegmentTransferCases(&v) != 0) return 1;

    return 0;
}
