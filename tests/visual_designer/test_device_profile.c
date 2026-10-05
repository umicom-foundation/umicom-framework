/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_device_profile.c
 *
 * PURPOSE:
 *   Validate describe preview device dimensions, density and input characteristics.
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
#include "umicom/designer/visual_designer/device_profile.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/device_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadDeviceProfileTransferEqual(const UmiRadDeviceProfile *a, const UmiRadDeviceProfile *b)
{
    return strcmp(a->profile_id, b->profile_id) == 0 &&
        a->width == b->width &&
        a->height == b->height &&
        a->dpi == b->dpi &&
        a->touch == b->touch;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadDeviceProfileTransferTails(UmiRadDeviceProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->profile_id) + 1U;
        memset(value->profile_id + used, 0xa5, sizeof(value->profile_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadDeviceProfileTransferMalformed(const UmiRadDeviceProfile *sample)
{
    (void)sample;
    {
        UmiRadDeviceProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.profile_id, 'x', sizeof(invalid.profile_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_device_profile_is_valid(&invalid)) ||
            umi_rad_device_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated profile_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadDeviceProfileTransferCases, UmiRadDeviceProfile,
    umi_rad_device_profile_archive_encode, umi_rad_device_profile_archive_decode,
    UmiRadDeviceProfileTransferEqual, UmiRadDeviceProfileTransferTails, UmiRadDeviceProfileTransferMalformed)

int main(void){UmiRadDeviceProfile item;CHECK(umi_rad_device_profile_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_device_profile_is_valid(&item));
    if (UmiRadDeviceProfileTransferCases(&item) != 0) return 1;
return 0;}
