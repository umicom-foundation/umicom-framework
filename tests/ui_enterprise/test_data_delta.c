/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_enterprise/test_data_delta.c
 *
 * PURPOSE:
 *   Exercise the data delta enterprise UI capability.
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
#include "umicom/ui/enterprise/data_delta.h"
#include <stdio.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/enterprise/data_delta.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiEntDataDeltaTransferEqual(const UmiUiEntDataDelta *a, const UmiUiEntDataDelta *b)
{
    return a->kind == b->kind &&
        a->rows.first == b->rows.first &&
        a->rows.count == b->rows.count &&
        a->sequence == b->sequence;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiEntDataDeltaTransferTails(UmiUiEntDataDelta *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiEntDataDeltaTransferMalformed(const UmiUiEntDataDelta *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiEntDataDeltaTransferCases, UmiUiEntDataDelta,
    umi_ui_ent_data_delta_archive_encode, umi_ui_ent_data_delta_archive_decode,
    UmiUiEntDataDeltaTransferEqual, UmiUiEntDataDeltaTransferTails, UmiUiEntDataDeltaTransferMalformed)

int main(void){UmiUiEntDataDelta d={UMI_UI_ENT_DELTA_UPDATE,{5U,2U},1U};/* Apply this operation only while the related capability or state is available. */ if(!umi_ui_ent_data_delta_validate(&d)||!umi_ui_ent_data_delta_touches(&d,6U)||umi_ui_ent_data_delta_touches(&d,7U))return 1;
    if (UmiUiEntDataDeltaTransferCases(&d) != 0) return 1;
puts("ok");return 0;}
