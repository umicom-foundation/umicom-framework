/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_breakpoint_preview.c
 *
 * PURPOSE:
 *   Validate resolve a named responsive preview breakpoint for the visual canvas.
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
#include "umicom/designer/visual_designer/breakpoint_preview.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/breakpoint_preview.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadBreakpointPreviewTransferEqual(const UmiRadBreakpointPreview *a, const UmiRadBreakpointPreview *b)
{
    return strcmp(a->breakpoint_id, b->breakpoint_id) == 0 &&
        a->viewport.width == b->viewport.width &&
        a->viewport.height == b->viewport.height &&
        a->dpi == b->dpi &&
        a->touch == b->touch;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadBreakpointPreviewTransferTails(UmiRadBreakpointPreview *value)
{
    (void)value;
    {
        size_t used = strlen(value->breakpoint_id) + 1U;
        memset(value->breakpoint_id + used, 0xa5, sizeof(value->breakpoint_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadBreakpointPreviewTransferMalformed(const UmiRadBreakpointPreview *sample)
{
    (void)sample;
    {
        UmiRadBreakpointPreview invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.breakpoint_id, 'x', sizeof(invalid.breakpoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_breakpoint_preview_is_valid(&invalid)) ||
            umi_rad_breakpoint_preview_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated breakpoint_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadBreakpointPreviewTransferCases, UmiRadBreakpointPreview,
    umi_rad_breakpoint_preview_archive_encode, umi_rad_breakpoint_preview_archive_decode,
    UmiRadBreakpointPreviewTransferEqual, UmiRadBreakpointPreviewTransferTails, UmiRadBreakpointPreviewTransferMalformed)

int main(void){UmiRadBreakpointPreview item;CHECK(umi_rad_breakpoint_preview_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_breakpoint_preview_is_valid(&item));
    if (UmiRadBreakpointPreviewTransferCases(&item) != 0) return 1;
return 0;}
