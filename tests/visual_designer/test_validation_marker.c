/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_validation_marker.c
 *
 * PURPOSE:
 *   Validate attach a validation severity/message to a component or property.
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
#include "umicom/designer/visual_designer/validation_marker.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/validation_marker.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadValidationMarkerTransferEqual(const UmiRadValidationMarker *a, const UmiRadValidationMarker *b)
{
    return strcmp(a->component_id, b->component_id) == 0 &&
        strcmp(a->property_id, b->property_id) == 0 &&
        a->severity == b->severity &&
        strcmp(a->message, b->message) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadValidationMarkerTransferTails(UmiRadValidationMarker *value)
{
    (void)value;
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
    {
        size_t used = strlen(value->property_id) + 1U;
        memset(value->property_id + used, 0xa5, sizeof(value->property_id) - used);
    }
    {
        size_t used = strlen(value->message) + 1U;
        memset(value->message + used, 0xa5, sizeof(value->message) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadValidationMarkerTransferMalformed(const UmiRadValidationMarker *sample)
{
    (void)sample;
    {
        UmiRadValidationMarker invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_validation_marker_is_valid(&invalid)) ||
            umi_rad_validation_marker_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadValidationMarker invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.property_id, 'x', sizeof(invalid.property_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_validation_marker_is_valid(&invalid)) ||
            umi_rad_validation_marker_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated property_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiRadValidationMarker invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message, 'x', sizeof(invalid.message));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_validation_marker_is_valid(&invalid)) ||
            umi_rad_validation_marker_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadValidationMarkerTransferCases, UmiRadValidationMarker,
    umi_rad_validation_marker_archive_encode, umi_rad_validation_marker_archive_decode,
    UmiRadValidationMarkerTransferEqual, UmiRadValidationMarkerTransferTails, UmiRadValidationMarkerTransferMalformed)

int main(void){UmiRadValidationMarker item;CHECK(umi_rad_validation_marker_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_validation_marker_is_valid(&item));
    if (UmiRadValidationMarkerTransferCases(&item) != 0) return 1;
return 0;}
