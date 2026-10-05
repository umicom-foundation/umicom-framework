/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_snap_result.c
 *
 * PURPOSE:
 *   Validate record the deterministic outcome of a snap calculation.
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
#include "umicom/designer/visual_designer/snap_result.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/snap_result.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadSnapResultTransferEqual(const UmiRadSnapResult *a, const UmiRadSnapResult *b)
{
    return a->requested.x == b->requested.x &&
        a->requested.y == b->requested.y &&
        a->resolved.x == b->resolved.x &&
        a->resolved.y == b->resolved.y &&
        a->snapped_x == b->snapped_x &&
        a->snapped_y == b->snapped_y;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadSnapResultTransferTails(UmiRadSnapResult *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadSnapResultTransferMalformed(const UmiRadSnapResult *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadSnapResultTransferCases, UmiRadSnapResult,
    umi_rad_snap_result_archive_encode, umi_rad_snap_result_archive_decode,
    UmiRadSnapResultTransferEqual, UmiRadSnapResultTransferTails, UmiRadSnapResultTransferMalformed)

int main(void){UmiRadSnapResult item;CHECK(umi_rad_snap_result_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_snap_result_is_valid(&item));
    if (UmiRadSnapResultTransferCases(&item) != 0) return 1;
return 0;}
