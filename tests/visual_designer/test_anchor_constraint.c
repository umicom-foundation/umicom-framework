/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_anchor_constraint.c
 *
 * PURPOSE:
 *   Validate describe edge anchors for adaptive layouts.
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
#include "umicom/designer/visual_designer/anchor_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/anchor_constraint.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadAnchorConstraintTransferEqual(const UmiRadAnchorConstraint *a, const UmiRadAnchorConstraint *b)
{
    return a->left == b->left &&
        a->top == b->top &&
        a->right == b->right &&
        a->bottom == b->bottom &&
        a->margin == b->margin;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadAnchorConstraintTransferTails(UmiRadAnchorConstraint *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadAnchorConstraintTransferMalformed(const UmiRadAnchorConstraint *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadAnchorConstraintTransferCases, UmiRadAnchorConstraint,
    umi_rad_anchor_constraint_archive_encode, umi_rad_anchor_constraint_archive_decode,
    UmiRadAnchorConstraintTransferEqual, UmiRadAnchorConstraintTransferTails, UmiRadAnchorConstraintTransferMalformed)

int main(void){UmiRadAnchorConstraint item;CHECK(umi_rad_anchor_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_anchor_constraint_is_valid(&item));
    if (UmiRadAnchorConstraintTransferCases(&item) != 0) return 1;
return 0;}
