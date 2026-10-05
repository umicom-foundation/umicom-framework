/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_insertion_marker.c
 *
 * PURPOSE:
 *   Validate represent insertion feedback within ordered containers.
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
#include "umicom/designer/visual_designer/insertion_marker.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/insertion_marker.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadInsertionMarkerTransferEqual(const UmiRadInsertionMarker *a, const UmiRadInsertionMarker *b)
{
    return strcmp(a->parent_id, b->parent_id) == 0 &&
        a->index == b->index &&
        a->bounds.x == b->bounds.x &&
        a->bounds.y == b->bounds.y &&
        a->bounds.width == b->bounds.width &&
        a->bounds.height == b->bounds.height &&
        a->visible == b->visible;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadInsertionMarkerTransferTails(UmiRadInsertionMarker *value)
{
    (void)value;
    {
        size_t used = strlen(value->parent_id) + 1U;
        memset(value->parent_id + used, 0xa5, sizeof(value->parent_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadInsertionMarkerTransferMalformed(const UmiRadInsertionMarker *sample)
{
    (void)sample;
    {
        UmiRadInsertionMarker invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.parent_id, 'x', sizeof(invalid.parent_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_insertion_marker_is_valid(&invalid)) ||
            umi_rad_insertion_marker_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated parent_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadInsertionMarkerTransferCases, UmiRadInsertionMarker,
    umi_rad_insertion_marker_archive_encode, umi_rad_insertion_marker_archive_decode,
    UmiRadInsertionMarkerTransferEqual, UmiRadInsertionMarkerTransferTails, UmiRadInsertionMarkerTransferMalformed)

int main(void){UmiRadInsertionMarker item;CHECK(umi_rad_insertion_marker_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_insertion_marker_is_valid(&item));
    if (UmiRadInsertionMarkerTransferCases(&item) != 0) return 1;
return 0;}
