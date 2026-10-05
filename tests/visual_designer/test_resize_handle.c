/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_resize_handle.c
 *
 * PURPOSE:
 *   Validate describe resize-handle semantics without depending on a toolkit cursor.
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
#include "umicom/designer/visual_designer/resize_handle.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/resize_handle.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadResizeHandleTransferEqual(const UmiRadResizeHandle *a, const UmiRadResizeHandle *b)
{
    return a->edges == b->edges &&
        a->location.x == b->location.x &&
        a->location.y == b->location.y &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadResizeHandleTransferTails(UmiRadResizeHandle *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadResizeHandleTransferMalformed(const UmiRadResizeHandle *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadResizeHandleTransferCases, UmiRadResizeHandle,
    umi_rad_resize_handle_archive_encode, umi_rad_resize_handle_archive_decode,
    UmiRadResizeHandleTransferEqual, UmiRadResizeHandleTransferTails, UmiRadResizeHandleTransferMalformed)

int main(void){UmiRadResizeHandle item;CHECK(umi_rad_resize_handle_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_resize_handle_is_valid(&item));
    if (UmiRadResizeHandleTransferCases(&item) != 0) return 1;
return 0;}
