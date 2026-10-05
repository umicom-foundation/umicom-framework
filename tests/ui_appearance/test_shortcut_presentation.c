/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_shortcut_presentation.c
 *
 * PURPOSE:
 *   Verify describe platform-neutral command shortcut hints for menus, toolbars and palettes.
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
#include "umicom/ui/appearance/shortcut_presentation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/shortcut_presentation.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceShortcutPresentationTransferEqual(const UmiAppearanceShortcutPresentation *a, const UmiAppearanceShortcutPresentation *b)
{
    return strcmp(a->action_id, b->action_id) == 0 &&
        strcmp(a->accelerator_id, b->accelerator_id) == 0 &&
        strcmp(a->display_text, b->display_text) == 0 &&
        a->discoverable == b->discoverable;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceShortcutPresentationTransferTails(UmiAppearanceShortcutPresentation *value)
{
    (void)value;
    {
        size_t used = strlen(value->action_id) + 1U;
        memset(value->action_id + used, 0xa5, sizeof(value->action_id) - used);
    }
    {
        size_t used = strlen(value->accelerator_id) + 1U;
        memset(value->accelerator_id + used, 0xa5, sizeof(value->accelerator_id) - used);
    }
    {
        size_t used = strlen(value->display_text) + 1U;
        memset(value->display_text + used, 0xa5, sizeof(value->display_text) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceShortcutPresentationTransferMalformed(const UmiAppearanceShortcutPresentation *sample)
{
    (void)sample;
    {
        UmiAppearanceShortcutPresentation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.action_id, 'x', sizeof(invalid.action_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_shortcut_presentation_is_valid(&invalid)) ||
            umi_appearance_shortcut_presentation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated action_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceShortcutPresentation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.accelerator_id, 'x', sizeof(invalid.accelerator_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_shortcut_presentation_is_valid(&invalid)) ||
            umi_appearance_shortcut_presentation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated accelerator_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceShortcutPresentation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_text, 'x', sizeof(invalid.display_text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_shortcut_presentation_is_valid(&invalid)) ||
            umi_appearance_shortcut_presentation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceShortcutPresentationTransferCases, UmiAppearanceShortcutPresentation,
    umi_appearance_shortcut_presentation_archive_encode, umi_appearance_shortcut_presentation_archive_decode,
    UmiAppearanceShortcutPresentationTransferEqual, UmiAppearanceShortcutPresentationTransferTails, UmiAppearanceShortcutPresentationTransferMalformed)

int main(void) {
    UmiAppearanceShortcutPresentation item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_shortcut_presentation_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_shortcut_presentation_is_valid(&item)) return 2;
    if (UmiAppearanceShortcutPresentationTransferCases(&item) != 0) return 1;

    return 0;
}
