/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_canvas_viewport.c
 *
 * PURPOSE:
 *   Validate track canvas origin, dimensions and zoom independently from document geometry.
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
#include "umicom/designer/visual_designer/canvas_viewport.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/canvas_viewport.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadCanvasViewportTransferEqual(const UmiRadCanvasViewport *a, const UmiRadCanvasViewport *b)
{
    return a->origin.x == b->origin.x &&
        a->origin.y == b->origin.y &&
        a->extent.width == b->extent.width &&
        a->extent.height == b->extent.height &&
        a->zoom == b->zoom;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadCanvasViewportTransferTails(UmiRadCanvasViewport *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadCanvasViewportTransferMalformed(const UmiRadCanvasViewport *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadCanvasViewportTransferCases, UmiRadCanvasViewport,
    umi_rad_canvas_viewport_archive_encode, umi_rad_canvas_viewport_archive_decode,
    UmiRadCanvasViewportTransferEqual, UmiRadCanvasViewportTransferTails, UmiRadCanvasViewportTransferMalformed)

int main(void){UmiRadCanvasViewport item;CHECK(umi_rad_canvas_viewport_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_canvas_viewport_is_valid(&item));
    if (UmiRadCanvasViewportTransferCases(&item) != 0) return 1;
return 0;}
