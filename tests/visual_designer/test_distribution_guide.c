/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_distribution_guide.c
 *
 * PURPOSE:
 *   Validate represent equal-spacing evidence for multiple selected components.
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
#include "umicom/designer/visual_designer/distribution_guide.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/distribution_guide.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadDistributionGuideTransferEqual(const UmiRadDistributionGuide *a, const UmiRadDistributionGuide *b)
{
    return strcmp(a->group_id, b->group_id) == 0 &&
        a->orientation == b->orientation &&
        a->spacing == b->spacing &&
        a->item_count == b->item_count;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadDistributionGuideTransferTails(UmiRadDistributionGuide *value)
{
    (void)value;
    {
        size_t used = strlen(value->group_id) + 1U;
        memset(value->group_id + used, 0xa5, sizeof(value->group_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadDistributionGuideTransferMalformed(const UmiRadDistributionGuide *sample)
{
    (void)sample;
    {
        UmiRadDistributionGuide invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.group_id, 'x', sizeof(invalid.group_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_distribution_guide_is_valid(&invalid)) ||
            umi_rad_distribution_guide_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated group_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadDistributionGuideTransferCases, UmiRadDistributionGuide,
    umi_rad_distribution_guide_archive_encode, umi_rad_distribution_guide_archive_decode,
    UmiRadDistributionGuideTransferEqual, UmiRadDistributionGuideTransferTails, UmiRadDistributionGuideTransferMalformed)

int main(void){UmiRadDistributionGuide item;CHECK(umi_rad_distribution_guide_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_distribution_guide_is_valid(&item));
    if (UmiRadDistributionGuideTransferCases(&item) != 0) return 1;
return 0;}
