/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_parent_constraint.c
 *
 * PURPOSE:
 *   Validate describe which semantic component families a parent slot accepts.
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
#include "umicom/designer/visual_designer/parent_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/parent_constraint.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadParentConstraintTransferEqual(const UmiRadParentConstraint *a, const UmiRadParentConstraint *b)
{
    return strcmp(a->parent_type, b->parent_type) == 0 &&
        strcmp(a->child_family, b->child_family) == 0 &&
        a->accepted == b->accepted;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadParentConstraintTransferTails(UmiRadParentConstraint *value)
{
    (void)value;
    {
        size_t used = strlen(value->parent_type) + 1U;
        memset(value->parent_type + used, 0xa5, sizeof(value->parent_type) - used);
    }
    {
        size_t used = strlen(value->child_family) + 1U;
        memset(value->child_family + used, 0xa5, sizeof(value->child_family) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadParentConstraintTransferMalformed(const UmiRadParentConstraint *sample)
{
    (void)sample;
    {
        UmiRadParentConstraint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_type, 'x', sizeof(invalid.parent_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_parent_constraint_is_valid(&invalid)) ||
            umi_rad_parent_constraint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_type was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadParentConstraint invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.child_family, 'x', sizeof(invalid.child_family));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_parent_constraint_is_valid(&invalid)) ||
            umi_rad_parent_constraint_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated child_family was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadParentConstraintTransferCases, UmiRadParentConstraint,
    umi_rad_parent_constraint_archive_encode, umi_rad_parent_constraint_archive_decode,
    UmiRadParentConstraintTransferEqual, UmiRadParentConstraintTransferTails, UmiRadParentConstraintTransferMalformed)

int main(void){UmiRadParentConstraint item;CHECK(umi_rad_parent_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_parent_constraint_is_valid(&item));
    if (UmiRadParentConstraintTransferCases(&item) != 0) return 1;
return 0;}
