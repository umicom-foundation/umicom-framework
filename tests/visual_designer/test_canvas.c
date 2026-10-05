/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_canvas.c
 *
 * PURPOSE:
 *   Validate describe a visual application design canvas and its revision state.
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
#include "umicom/designer/visual_designer/canvas.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/canvas.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadCanvasTransferEqual(const UmiRadCanvas *a, const UmiRadCanvas *b)
{
    return strcmp(a->document_id, b->document_id) == 0 &&
        strcmp(a->root_component_id, b->root_component_id) == 0 &&
        a->revision == b->revision &&
        a->dirty == b->dirty;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadCanvasTransferTails(UmiRadCanvas *value)
{
    (void)value;
    {
        size_t used = strlen(value->document_id) + 1U;
        memset(value->document_id + used, 0xa5, sizeof(value->document_id) - used);
    }
    {
        size_t used = strlen(value->root_component_id) + 1U;
        memset(value->root_component_id + used, 0xa5, sizeof(value->root_component_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadCanvasTransferMalformed(const UmiRadCanvas *sample)
{
    (void)sample;
    {
        UmiRadCanvas invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.document_id, 'x', sizeof(invalid.document_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_canvas_is_valid(&invalid)) ||
            umi_rad_canvas_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated document_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadCanvas invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.root_component_id, 'x', sizeof(invalid.root_component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_canvas_is_valid(&invalid)) ||
            umi_rad_canvas_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated root_component_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadCanvasTransferCases, UmiRadCanvas,
    umi_rad_canvas_archive_encode, umi_rad_canvas_archive_decode,
    UmiRadCanvasTransferEqual, UmiRadCanvasTransferTails, UmiRadCanvasTransferMalformed)

int main(void){UmiRadCanvas item;CHECK(umi_rad_canvas_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_canvas_is_valid(&item));
    if (UmiRadCanvasTransferCases(&item) != 0) return 1;
return 0;}
