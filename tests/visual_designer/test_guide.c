/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_guide.c
 *
 * PURPOSE:
 *   Validate represent user-created horizontal and vertical design guides.
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
#include "umicom/designer/visual_designer/guide.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/guide.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadGuideTransferEqual(const UmiRadGuide *a, const UmiRadGuide *b)
{
    return strcmp(a->guide_id, b->guide_id) == 0 &&
        a->orientation == b->orientation &&
        a->position == b->position &&
        a->locked == b->locked;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadGuideTransferTails(UmiRadGuide *value)
{
    (void)value;
    {
        size_t used = strlen(value->guide_id) + 1U;
        memset(value->guide_id + used, 0xa5, sizeof(value->guide_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadGuideTransferMalformed(const UmiRadGuide *sample)
{
    (void)sample;
    {
        UmiRadGuide invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.guide_id, 'x', sizeof(invalid.guide_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_guide_is_valid(&invalid)) ||
            umi_rad_guide_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated guide_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadGuideTransferCases, UmiRadGuide,
    umi_rad_guide_archive_encode, umi_rad_guide_archive_decode,
    UmiRadGuideTransferEqual, UmiRadGuideTransferTails, UmiRadGuideTransferMalformed)

int main(void){UmiRadGuide item;CHECK(umi_rad_guide_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_guide_is_valid(&item));
    if (UmiRadGuideTransferCases(&item) != 0) return 1;
return 0;}
