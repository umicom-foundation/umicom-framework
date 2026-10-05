/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_component_state_projection.c
 *
 * PURPOSE:
 *   Verify map semantic component state to resolved style and accessibility state identifiers.
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
#include "umicom/ui/appearance/component_state_projection.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/component_state_projection.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceComponentStateProjectionTransferEqual(const UmiAppearanceComponentStateProjection *a, const UmiAppearanceComponentStateProjection *b)
{
    return strcmp(a->component_id, b->component_id) == 0 &&
        strcmp(a->state_id, b->state_id) == 0 &&
        strcmp(a->resolved_style_id, b->resolved_style_id) == 0 &&
        a->focus_visible == b->focus_visible &&
        a->disabled == b->disabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceComponentStateProjectionTransferTails(UmiAppearanceComponentStateProjection *value)
{
    (void)value;
    {
        size_t used = strlen(value->component_id) + 1U;
        memset(value->component_id + used, 0xa5, sizeof(value->component_id) - used);
    }
    {
        size_t used = strlen(value->state_id) + 1U;
        memset(value->state_id + used, 0xa5, sizeof(value->state_id) - used);
    }
    {
        size_t used = strlen(value->resolved_style_id) + 1U;
        memset(value->resolved_style_id + used, 0xa5, sizeof(value->resolved_style_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceComponentStateProjectionTransferMalformed(const UmiAppearanceComponentStateProjection *sample)
{
    (void)sample;
    {
        UmiAppearanceComponentStateProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.component_id, 'x', sizeof(invalid.component_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_component_state_projection_is_valid(&invalid)) ||
            umi_appearance_component_state_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated component_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceComponentStateProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.state_id, 'x', sizeof(invalid.state_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_component_state_projection_is_valid(&invalid)) ||
            umi_appearance_component_state_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated state_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiAppearanceComponentStateProjection invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.resolved_style_id, 'x', sizeof(invalid.resolved_style_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_component_state_projection_is_valid(&invalid)) ||
            umi_appearance_component_state_projection_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated resolved_style_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceComponentStateProjectionTransferCases, UmiAppearanceComponentStateProjection,
    umi_appearance_component_state_projection_archive_encode, umi_appearance_component_state_projection_archive_decode,
    UmiAppearanceComponentStateProjectionTransferEqual, UmiAppearanceComponentStateProjectionTransferTails, UmiAppearanceComponentStateProjectionTransferMalformed)

int main(void) {
    UmiAppearanceComponentStateProjection item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_component_state_projection_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_component_state_projection_is_valid(&item)) return 2;
    if (UmiAppearanceComponentStateProjectionTransferCases(&item) != 0) return 1;

    return 0;
}
