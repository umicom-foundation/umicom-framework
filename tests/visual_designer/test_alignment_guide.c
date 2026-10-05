/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_alignment_guide.c
 *
 * PURPOSE:
 *   Validate represent alignment evidence between visual components.
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
#include "umicom/designer/visual_designer/alignment_guide.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/alignment_guide.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadAlignmentGuideTransferEqual(const UmiRadAlignmentGuide *a, const UmiRadAlignmentGuide *b)
{
    return strcmp(a->source_id, b->source_id) == 0 &&
        strcmp(a->peer_id, b->peer_id) == 0 &&
        a->orientation == b->orientation &&
        a->position == b->position;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadAlignmentGuideTransferTails(UmiRadAlignmentGuide *value)
{
    (void)value;
    {
        size_t used = strlen(value->source_id) + 1U;
        memset(value->source_id + used, 0xa5, sizeof(value->source_id) - used);
    }
    {
        size_t used = strlen(value->peer_id) + 1U;
        memset(value->peer_id + used, 0xa5, sizeof(value->peer_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadAlignmentGuideTransferMalformed(const UmiRadAlignmentGuide *sample)
{
    (void)sample;
    {
        UmiRadAlignmentGuide invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.source_id, 'x', sizeof(invalid.source_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_alignment_guide_is_valid(&invalid)) ||
            umi_rad_alignment_guide_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated source_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadAlignmentGuide invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.peer_id, 'x', sizeof(invalid.peer_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_alignment_guide_is_valid(&invalid)) ||
            umi_rad_alignment_guide_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated peer_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadAlignmentGuideTransferCases, UmiRadAlignmentGuide,
    umi_rad_alignment_guide_archive_encode, umi_rad_alignment_guide_archive_decode,
    UmiRadAlignmentGuideTransferEqual, UmiRadAlignmentGuideTransferTails, UmiRadAlignmentGuideTransferMalformed)

int main(void){UmiRadAlignmentGuide item;CHECK(umi_rad_alignment_guide_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_alignment_guide_is_valid(&item));
    if (UmiRadAlignmentGuideTransferCases(&item) != 0) return 1;
return 0;}
