/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_responsive_variant.c
 *
 * PURPOSE:
 *   Validate describe per-breakpoint component geometry and visibility overrides.
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
#include "umicom/designer/visual_designer/responsive_variant.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/designer/visual_designer/responsive_variant.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRadResponsiveVariantTransferEqual(const UmiRadResponsiveVariant *a, const UmiRadResponsiveVariant *b)
{
    return strcmp(a->breakpoint_id, b->breakpoint_id) == 0 &&
        a->bounds.x == b->bounds.x &&
        a->bounds.y == b->bounds.y &&
        a->bounds.width == b->bounds.width &&
        a->bounds.height == b->bounds.height &&
        a->visible == b->visible &&
        a->override_geometry == b->override_geometry;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRadResponsiveVariantTransferTails(UmiRadResponsiveVariant *value)
{
    (void)value;
    {
        size_t used = strlen(value->breakpoint_id) + 1U;
        memset(value->breakpoint_id + used, 0xa5, sizeof(value->breakpoint_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRadResponsiveVariantTransferMalformed(const UmiRadResponsiveVariant *sample)
{
    (void)sample;
    {
        UmiRadResponsiveVariant invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.breakpoint_id, 'x', sizeof(invalid.breakpoint_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_rad_responsive_variant_is_valid(&invalid)) ||
            umi_rad_responsive_variant_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated breakpoint_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRadResponsiveVariantTransferCases, UmiRadResponsiveVariant,
    umi_rad_responsive_variant_archive_encode, umi_rad_responsive_variant_archive_decode,
    UmiRadResponsiveVariantTransferEqual, UmiRadResponsiveVariantTransferTails, UmiRadResponsiveVariantTransferMalformed)

int main(void){UmiRadResponsiveVariant item;CHECK(umi_rad_responsive_variant_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_responsive_variant_is_valid(&item));
    if (UmiRadResponsiveVariantTransferCases(&item) != 0) return 1;
return 0;}
