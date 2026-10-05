/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_zoom.c
 *
 * PURPOSE:
 *   Validate provide bounded zoom policy for visual authoring surfaces.
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
#include "umicom/designer/visual_designer/zoom.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/zoom.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadZoomPolicyTransferEqual(const UmiRadZoomPolicy *a, const UmiRadZoomPolicy *b)
{
    return a->minimum == b->minimum &&
        a->maximum == b->maximum &&
        a->current == b->current;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadZoomPolicyTransferTails(UmiRadZoomPolicy *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadZoomPolicyTransferMalformed(const UmiRadZoomPolicy *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadZoomPolicyTransferCases, UmiRadZoomPolicy,
    umi_rad_zoom_archive_encode, umi_rad_zoom_archive_decode,
    UmiRadZoomPolicyTransferEqual, UmiRadZoomPolicyTransferTails, UmiRadZoomPolicyTransferMalformed)

int main(void){UmiRadZoomPolicy item;CHECK(umi_rad_zoom_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_zoom_is_valid(&item));
    if (UmiRadZoomPolicyTransferCases(&item) != 0) return 1;
return 0;}
