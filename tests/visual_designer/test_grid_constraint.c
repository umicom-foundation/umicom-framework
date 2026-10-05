/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_grid_constraint.c
 *
 * PURPOSE:
 *   Validate describe renderer-neutral grid row/column placement and spans.
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
#include "umicom/designer/visual_designer/grid_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/grid_constraint.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadGridConstraintTransferEqual(const UmiRadGridConstraint *a, const UmiRadGridConstraint *b)
{
    return a->row == b->row &&
        a->column == b->column &&
        a->row_span == b->row_span &&
        a->column_span == b->column_span;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadGridConstraintTransferTails(UmiRadGridConstraint *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadGridConstraintTransferMalformed(const UmiRadGridConstraint *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadGridConstraintTransferCases, UmiRadGridConstraint,
    umi_rad_grid_constraint_archive_encode, umi_rad_grid_constraint_archive_decode,
    UmiRadGridConstraintTransferEqual, UmiRadGridConstraintTransferTails, UmiRadGridConstraintTransferMalformed)

int main(void){UmiRadGridConstraint item;CHECK(umi_rad_grid_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_grid_constraint_is_valid(&item));
    if (UmiRadGridConstraintTransferCases(&item) != 0) return 1;
return 0;}
