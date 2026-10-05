/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_flex_constraint.c
 *
 * PURPOSE:
 *   Validate describe renderer-neutral flexible-box growth and basis constraints.
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
#include "umicom/designer/visual_designer/flex_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/flex_constraint.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadFlexConstraintTransferEqual(const UmiRadFlexConstraint *a, const UmiRadFlexConstraint *b)
{
    return a->grow == b->grow &&
        a->shrink == b->shrink &&
        a->basis == b->basis &&
        a->order == b->order;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadFlexConstraintTransferTails(UmiRadFlexConstraint *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadFlexConstraintTransferMalformed(const UmiRadFlexConstraint *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadFlexConstraintTransferCases, UmiRadFlexConstraint,
    umi_rad_flex_constraint_archive_encode, umi_rad_flex_constraint_archive_decode,
    UmiRadFlexConstraintTransferEqual, UmiRadFlexConstraintTransferTails, UmiRadFlexConstraintTransferMalformed)

int main(void){UmiRadFlexConstraint item;CHECK(umi_rad_flex_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_flex_constraint_is_valid(&item));
    if (UmiRadFlexConstraintTransferCases(&item) != 0) return 1;
return 0;}
