/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_theme_scope.c
 *
 * PURPOSE:
 *   Verify describe the semantic scope at which a theme override is applied.
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
#include "umicom/ui/appearance/theme_scope.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/theme_scope.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceThemeScopeTransferEqual(const UmiAppearanceThemeScope *a, const UmiAppearanceThemeScope *b)
{
    return strcmp(a->scope_id, b->scope_id) == 0 &&
        a->scope == b->scope &&
        strcmp(a->owner_id, b->owner_id) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceThemeScopeTransferTails(UmiAppearanceThemeScope *value)
{
    (void)value;
    {
        size_t used = strlen(value->scope_id) + 1U;
        memset(value->scope_id + used, 0xa5, sizeof(value->scope_id) - used);
    }
    {
        size_t used = strlen(value->owner_id) + 1U;
        memset(value->owner_id + used, 0xa5, sizeof(value->owner_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceThemeScopeTransferMalformed(const UmiAppearanceThemeScope *sample)
{
    (void)sample;
    {
        UmiAppearanceThemeScope invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.scope_id, 'x', sizeof(invalid.scope_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_theme_scope_is_valid(&invalid)) ||
            umi_appearance_theme_scope_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated scope_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceThemeScope invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.owner_id, 'x', sizeof(invalid.owner_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_theme_scope_is_valid(&invalid)) ||
            umi_appearance_theme_scope_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated owner_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceThemeScopeTransferCases, UmiAppearanceThemeScope,
    umi_appearance_theme_scope_archive_encode, umi_appearance_theme_scope_archive_decode,
    UmiAppearanceThemeScopeTransferEqual, UmiAppearanceThemeScopeTransferTails, UmiAppearanceThemeScopeTransferMalformed)

int main(void) {
    UmiAppearanceThemeScope item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_theme_scope_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_theme_scope_is_valid(&item)) return 2;
    if (UmiAppearanceThemeScopeTransferCases(&item) != 0) return 1;

    return 0;
}
