/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_row_height.c
 *
 * PURPOSE:
 *   Exercise the row height enterprise UI capability.
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
#include "umicom/ui/enterprise/row_height.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/row_height.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntRowHeightTransferEqual(const UmiUiEntRowHeight *a, const UmiUiEntRowHeight *b)
{
    return a->preferred == b->preferred &&
        a->minimum == b->minimum &&
        a->maximum == b->maximum &&
        a->automatic == b->automatic;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntRowHeightTransferTails(UmiUiEntRowHeight *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntRowHeightTransferMalformed(const UmiUiEntRowHeight *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntRowHeightTransferCases, UmiUiEntRowHeight,
    umi_ui_ent_row_height_archive_encode, umi_ui_ent_row_height_archive_decode,
    UmiUiEntRowHeightTransferEqual, UmiUiEntRowHeightTransferTails, UmiUiEntRowHeightTransferMalformed)

int main(void){UmiUiEntRowHeight v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_row_height_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_row_height_validate(&v))return 9;
    if (UmiUiEntRowHeightTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
