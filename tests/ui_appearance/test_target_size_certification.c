/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_target_size_certification.c
 *
 * PURPOSE:
 *   Verify certify resolved interactive target dimensions against modality-specific accessibility policy.
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
#include "umicom/ui/appearance/target_size_certification.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/target_size_certification.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceTargetSizeCertificationTransferEqual(const UmiAppearanceTargetSizeCertification *a, const UmiAppearanceTargetSizeCertification *b)
{
    return strcmp(a->target_id, b->target_id) == 0 &&
        a->width_dp == b->width_dp &&
        a->height_dp == b->height_dp &&
        a->required_width_dp == b->required_width_dp &&
        a->required_height_dp == b->required_height_dp &&
        a->passed == b->passed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceTargetSizeCertificationTransferTails(UmiAppearanceTargetSizeCertification *value)
{
    (void)value;
    {
        size_t used = strlen(value->target_id) + 1U;
        memset(value->target_id + used, 0xa5, sizeof(value->target_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceTargetSizeCertificationTransferMalformed(const UmiAppearanceTargetSizeCertification *sample)
{
    (void)sample;
    {
        UmiAppearanceTargetSizeCertification invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.target_id, 'x', sizeof(invalid.target_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_target_size_certification_is_valid(&invalid)) ||
            umi_appearance_target_size_certification_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated target_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceTargetSizeCertificationTransferCases, UmiAppearanceTargetSizeCertification,
    umi_appearance_target_size_certification_archive_encode, umi_appearance_target_size_certification_archive_decode,
    UmiAppearanceTargetSizeCertificationTransferEqual, UmiAppearanceTargetSizeCertificationTransferTails, UmiAppearanceTargetSizeCertificationTransferMalformed)

int main(void) {
    UmiAppearanceTargetSizeCertification item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_target_size_certification_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_target_size_certification_is_valid(&item)) return 2;
    if (UmiAppearanceTargetSizeCertificationTransferCases(&item) != 0) return 1;

    item.width_dp=20.0; umi_appearance_target_size_certification_evaluate(&item); /* Apply this branch only when its contract condition is satisfied. */ if(item.passed) return 3;
    return 0;
}
