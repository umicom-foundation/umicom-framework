/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_pan.c
 *
 * PURPOSE:
 *   Validate provide deterministic canvas panning state.
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
#include "umicom/designer/visual_designer/pan.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/pan.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadPanStateTransferEqual(const UmiRadPanState *a, const UmiRadPanState *b)
{
    return a->offset.x == b->offset.x &&
        a->offset.y == b->offset.y &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadPanStateTransferTails(UmiRadPanState *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadPanStateTransferMalformed(const UmiRadPanState *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadPanStateTransferCases, UmiRadPanState,
    umi_rad_pan_archive_encode, umi_rad_pan_archive_decode,
    UmiRadPanStateTransferEqual, UmiRadPanStateTransferTails, UmiRadPanStateTransferMalformed)

int main(void){UmiRadPanState item;CHECK(umi_rad_pan_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_pan_is_valid(&item));
    if (UmiRadPanStateTransferCases(&item) != 0) return 1;
return 0;}
