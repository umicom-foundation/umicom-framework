/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_preview_state.c
 *
 * PURPOSE:
 *   Validate record renderer-neutral preview health and diagnostic counts.
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
#include "umicom/designer/visual_designer/preview_state.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/preview_state.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadPreviewStateTransferEqual(const UmiRadPreviewState *a, const UmiRadPreviewState *b)
{
    return a->document_revision == b->document_revision &&
        a->render_revision == b->render_revision &&
        a->warning_count == b->warning_count &&
        a->error_count == b->error_count &&
        a->healthy == b->healthy;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadPreviewStateTransferTails(UmiRadPreviewState *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadPreviewStateTransferMalformed(const UmiRadPreviewState *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadPreviewStateTransferCases, UmiRadPreviewState,
    umi_rad_preview_state_archive_encode, umi_rad_preview_state_archive_decode,
    UmiRadPreviewStateTransferEqual, UmiRadPreviewStateTransferTails, UmiRadPreviewStateTransferMalformed)

int main(void){UmiRadPreviewState item;CHECK(umi_rad_preview_state_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_preview_state_is_valid(&item));
    if (UmiRadPreviewStateTransferCases(&item) != 0) return 1;
return 0;}
