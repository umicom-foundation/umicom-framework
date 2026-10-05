/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_snap_policy.c
 *
 * PURPOSE:
 *   Validate configure grid, guide and component snapping tolerance.
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
#include "umicom/designer/visual_designer/snap_policy.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/snap_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadSnapPolicyTransferEqual(const UmiRadSnapPolicy *a, const UmiRadSnapPolicy *b)
{
    return a->grid_enabled == b->grid_enabled &&
        a->guides_enabled == b->guides_enabled &&
        a->components_enabled == b->components_enabled &&
        a->tolerance == b->tolerance;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadSnapPolicyTransferTails(UmiRadSnapPolicy *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadSnapPolicyTransferMalformed(const UmiRadSnapPolicy *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadSnapPolicyTransferCases, UmiRadSnapPolicy,
    umi_rad_snap_policy_archive_encode, umi_rad_snap_policy_archive_decode,
    UmiRadSnapPolicyTransferEqual, UmiRadSnapPolicyTransferTails, UmiRadSnapPolicyTransferMalformed)

int main(void){UmiRadSnapPolicy item;CHECK(umi_rad_snap_policy_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_snap_policy_is_valid(&item));
    if (UmiRadSnapPolicyTransferCases(&item) != 0) return 1;
return 0;}
