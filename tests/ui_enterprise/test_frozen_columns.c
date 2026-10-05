/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_frozen_columns.c
 *
 * PURPOSE:
 *   Exercise the frozen columns enterprise UI capability.
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
#include "umicom/ui/enterprise/frozen_columns.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/frozen_columns.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntFrozenColumnsTransferEqual(const UmiUiEntFrozenColumns *a, const UmiUiEntFrozenColumns *b)
{
    return a->leading_count == b->leading_count &&
        a->trailing_count == b->trailing_count &&
        a->total_columns == b->total_columns;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntFrozenColumnsTransferTails(UmiUiEntFrozenColumns *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntFrozenColumnsTransferMalformed(const UmiUiEntFrozenColumns *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntFrozenColumnsTransferCases, UmiUiEntFrozenColumns,
    umi_ui_ent_frozen_columns_archive_encode, umi_ui_ent_frozen_columns_archive_decode,
    UmiUiEntFrozenColumnsTransferEqual, UmiUiEntFrozenColumnsTransferTails, UmiUiEntFrozenColumnsTransferMalformed)

int main(void){UmiUiEntFrozenColumns v;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_ui_ent_frozen_columns_init(&v)!=UMI_STATUS_OK)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!umi_ui_ent_frozen_columns_validate(&v))return 9;
    if (UmiUiEntFrozenColumnsTransferCases(&v) != 0) return 1;
puts("ok");return 0;}
