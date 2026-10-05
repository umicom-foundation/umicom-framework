/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_preview_target.c
 *
 * PURPOSE:
 *   Validate describe GTK4, Qt6, Native Web or abstract-device preview targets.
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
#include "umicom/designer/visual_designer/preview_target.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/preview_target.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadPreviewTargetTransferEqual(const UmiRadPreviewTarget *a, const UmiRadPreviewTarget *b)
{
    return strcmp(a->target_id, b->target_id) == 0 &&
        a->kind == b->kind &&
        a->viewport.width == b->viewport.width &&
        a->viewport.height == b->viewport.height &&
        a->dpi == b->dpi;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadPreviewTargetTransferTails(UmiRadPreviewTarget *value)
{
    (void)value;
    {
        size_t used = strlen(value->target_id) + 1U;
        memset(value->target_id + used, 0xa5, sizeof(value->target_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadPreviewTargetTransferMalformed(const UmiRadPreviewTarget *sample)
{
    (void)sample;
    {
        UmiRadPreviewTarget invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_id, 'x', sizeof(invalid.target_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_preview_target_is_valid(&invalid)) ||
            umi_rad_preview_target_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadPreviewTargetTransferCases, UmiRadPreviewTarget,
    umi_rad_preview_target_archive_encode, umi_rad_preview_target_archive_decode,
    UmiRadPreviewTargetTransferEqual, UmiRadPreviewTargetTransferTails, UmiRadPreviewTargetTransferMalformed)

int main(void){UmiRadPreviewTarget item;CHECK(umi_rad_preview_target_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_preview_target_is_valid(&item));
    if (UmiRadPreviewTargetTransferCases(&item) != 0) return 1;
return 0;}
