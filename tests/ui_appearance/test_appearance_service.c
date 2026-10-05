/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_service.c
 *
 * PURPOSE:
 *   Verify expose aggregate readiness for Framework-owned production appearance services consumed by every thin application.
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
#include "umicom/ui/appearance/appearance_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/appearance_service.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceAppearanceServiceTransferEqual(const UmiAppearanceAppearanceService *a, const UmiAppearanceAppearanceService *b)
{
    return strcmp(a->service_id, b->service_id) == 0 &&
        a->themes_ready == b->themes_ready &&
        a->typography_ready == b->typography_ready &&
        a->scaling_ready == b->scaling_ready &&
        a->accessibility_ready == b->accessibility_ready &&
        a->renderers_ready == b->renderers_ready &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceAppearanceServiceTransferTails(UmiAppearanceAppearanceService *value)
{
    (void)value;
    {
        size_t used = strlen(value->service_id) + 1U;
        memset(value->service_id + used, 0xa5, sizeof(value->service_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceAppearanceServiceTransferMalformed(const UmiAppearanceAppearanceService *sample)
{
    (void)sample;
    {
        UmiAppearanceAppearanceService invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.service_id, 'x', sizeof(invalid.service_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_service_is_valid(&invalid)) ||
            umi_appearance_service_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated service_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceAppearanceServiceTransferCases, UmiAppearanceAppearanceService,
    umi_appearance_service_archive_encode, umi_appearance_service_archive_decode,
    UmiAppearanceAppearanceServiceTransferEqual, UmiAppearanceAppearanceServiceTransferTails, UmiAppearanceAppearanceServiceTransferMalformed)

int main(void) {
    UmiAppearanceAppearanceService item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_service_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_service_is_valid(&item)) return 2;
    if (UmiAppearanceAppearanceServiceTransferCases(&item) != 0) return 1;

    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_service_ready(&item)) return 3;
    return 0;
}
