/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_renderer_theme_projection.c
 *
 * PURPOSE:
 *   Verify record one renderer-specific projection of a semantic style packet without transferring state ownership.
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
#include "umicom/ui/appearance/renderer_theme_projection.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/renderer_theme_projection.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceRendererThemeProjectionTransferEqual(const UmiAppearanceRendererThemeProjection *a, const UmiAppearanceRendererThemeProjection *b)
{
    return strcmp(a->projection_id, b->projection_id) == 0 &&
        strcmp(a->packet_id, b->packet_id) == 0 &&
        a->renderer == b->renderer &&
        a->semantic_revision == b->semantic_revision &&
        a->projected_revision == b->projected_revision &&
        a->complete == b->complete;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceRendererThemeProjectionTransferTails(UmiAppearanceRendererThemeProjection *value)
{
    (void)value;
    {
        size_t used = strlen(value->projection_id) + 1U;
        memset(value->projection_id + used, 0xa5, sizeof(value->projection_id) - used);
    }
    {
        size_t used = strlen(value->packet_id) + 1U;
        memset(value->packet_id + used, 0xa5, sizeof(value->packet_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceRendererThemeProjectionTransferMalformed(const UmiAppearanceRendererThemeProjection *sample)
{
    (void)sample;
    {
        UmiAppearanceRendererThemeProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.projection_id, 'x', sizeof(invalid.projection_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_renderer_theme_projection_is_valid(&invalid)) ||
            umi_appearance_renderer_theme_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated projection_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceRendererThemeProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.packet_id, 'x', sizeof(invalid.packet_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_renderer_theme_projection_is_valid(&invalid)) ||
            umi_appearance_renderer_theme_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated packet_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceRendererThemeProjectionTransferCases, UmiAppearanceRendererThemeProjection,
    umi_appearance_renderer_theme_projection_archive_encode, umi_appearance_renderer_theme_projection_archive_decode,
    UmiAppearanceRendererThemeProjectionTransferEqual, UmiAppearanceRendererThemeProjectionTransferTails, UmiAppearanceRendererThemeProjectionTransferMalformed)

int main(void) {
    UmiAppearanceRendererThemeProjection item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_renderer_theme_projection_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_renderer_theme_projection_is_valid(&item)) return 2;
    if (UmiAppearanceRendererThemeProjectionTransferCases(&item) != 0) return 1;

    return 0;
}
