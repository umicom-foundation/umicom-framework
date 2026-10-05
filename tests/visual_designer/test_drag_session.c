/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_drag_session.c
 *
 * PURPOSE:
 *   Validate track a visual component drag operation from press through commit/cancel.
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
#include "umicom/designer/visual_designer/drag_session.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/drag_session.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadDragSessionTransferEqual(const UmiRadDragSession *a, const UmiRadDragSession *b)
{
    return strcmp(a->component_id, b->component_id) == 0 &&
        a->start.x == b->start.x &&
        a->start.y == b->start.y &&
        a->current.x == b->current.x &&
        a->current.y == b->current.y &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadDragSessionTransferTails(UmiRadDragSession *value)
{
    (void)value;
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadDragSessionTransferMalformed(const UmiRadDragSession *sample)
{
    (void)sample;
    {
        UmiRadDragSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_drag_session_is_valid(&invalid)) ||
            umi_rad_drag_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadDragSessionTransferCases, UmiRadDragSession,
    umi_rad_drag_session_archive_encode, umi_rad_drag_session_archive_decode,
    UmiRadDragSessionTransferEqual, UmiRadDragSessionTransferTails, UmiRadDragSessionTransferMalformed)

int main(void){UmiRadDragSession item;CHECK(umi_rad_drag_session_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_drag_session_is_valid(&item));
    if (UmiRadDragSessionTransferCases(&item) != 0) return 1;
return 0;}
