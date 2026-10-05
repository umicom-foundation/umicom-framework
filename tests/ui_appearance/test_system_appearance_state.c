/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_system_appearance_state.c
 *
 * PURPOSE:
 *   Verify represent operating-system appearance signals without coupling Framework logic to platform APIs.
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
#include "umicom/ui/appearance/system_appearance_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/system_appearance_state.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceSystemAppearanceStateTransferEqual(const UmiAppearanceSystemAppearanceState *a, const UmiAppearanceSystemAppearanceState *b)
{
    return strcmp(a->system_id, b->system_id) == 0 &&
        a->dark_mode == b->dark_mode &&
        a->high_contrast == b->high_contrast &&
        a->reduced_motion == b->reduced_motion &&
        a->dpi == b->dpi &&
        a->scale == b->scale;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceSystemAppearanceStateTransferTails(UmiAppearanceSystemAppearanceState *value)
{
    (void)value;
    {
        size_t used = strlen(value->system_id) + 1U;
        memset(value->system_id + used, 0xa5, sizeof(value->system_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceSystemAppearanceStateTransferMalformed(const UmiAppearanceSystemAppearanceState *sample)
{
    (void)sample;
    {
        UmiAppearanceSystemAppearanceState invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.system_id, 'x', sizeof(invalid.system_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_system_appearance_state_is_valid(&invalid)) ||
            umi_appearance_system_appearance_state_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated system_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceSystemAppearanceStateTransferCases, UmiAppearanceSystemAppearanceState,
    umi_appearance_system_appearance_state_archive_encode, umi_appearance_system_appearance_state_archive_decode,
    UmiAppearanceSystemAppearanceStateTransferEqual, UmiAppearanceSystemAppearanceStateTransferTails, UmiAppearanceSystemAppearanceStateTransferMalformed)

int main(void) {
    UmiAppearanceSystemAppearanceState item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_system_appearance_state_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_system_appearance_state_is_valid(&item)) return 2;
    if (UmiAppearanceSystemAppearanceStateTransferCases(&item) != 0) return 1;

    return 0;
}
