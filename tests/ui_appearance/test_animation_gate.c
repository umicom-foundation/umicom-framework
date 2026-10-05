/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_animation_gate.c
 *
 * PURPOSE:
 *   Verify decide whether an animation may run after reduced-motion and essential-feedback policy is applied.
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
#include "umicom/ui/appearance/animation_gate.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/animation_gate.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceAnimationGateTransferEqual(const UmiAppearanceAnimationGate *a, const UmiAppearanceAnimationGate *b)
{
    return strcmp(a->animation_id, b->animation_id) == 0 &&
        a->essential == b->essential &&
        a->reduced_motion == b->reduced_motion &&
        a->allowed == b->allowed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceAnimationGateTransferTails(UmiAppearanceAnimationGate *value)
{
    (void)value;
    {
        size_t used = strlen(value->animation_id) + 1U;
        memset(value->animation_id + used, 0xa5, sizeof(value->animation_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceAnimationGateTransferMalformed(const UmiAppearanceAnimationGate *sample)
{
    (void)sample;
    {
        UmiAppearanceAnimationGate invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.animation_id, 'x', sizeof(invalid.animation_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_animation_gate_is_valid(&invalid)) ||
            umi_appearance_animation_gate_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated animation_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceAnimationGateTransferCases, UmiAppearanceAnimationGate,
    umi_appearance_animation_gate_archive_encode, umi_appearance_animation_gate_archive_decode,
    UmiAppearanceAnimationGateTransferEqual, UmiAppearanceAnimationGateTransferTails, UmiAppearanceAnimationGateTransferMalformed)

int main(void) {
    UmiAppearanceAnimationGate item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_animation_gate_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_animation_gate_is_valid(&item)) return 2;
    if (UmiAppearanceAnimationGateTransferCases(&item) != 0) return 1;

    item.reduced_motion=true; item.essential=false; umi_appearance_animation_gate_resolve(&item); /* Apply this operation only while the related capability or state is available. */ if(item.allowed) return 3;
    return 0;
}
