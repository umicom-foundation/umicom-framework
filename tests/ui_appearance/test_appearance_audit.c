/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_audit.c
 *
 * PURPOSE:
 *   Verify aggregate appearance accessibility, scaling, typography and renderer-parity findings into one audit result.
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
#include "umicom/ui/appearance/appearance_audit.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/appearance/appearance_audit.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAppearanceAppearanceAuditTransferEqual(const UmiAppearanceAppearanceAudit *a, const UmiAppearanceAppearanceAudit *b)
{
    return strcmp(a->audit_id, b->audit_id) == 0 &&
        a->checks == b->checks &&
        a->warnings == b->warnings &&
        a->errors == b->errors &&
        a->passed == b->passed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAppearanceAppearanceAuditTransferTails(UmiAppearanceAppearanceAudit *value)
{
    (void)value;
    {
        size_t used = strlen(value->audit_id) + 1U;
        memset(value->audit_id + used, 0xa5, sizeof(value->audit_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAppearanceAppearanceAuditTransferMalformed(const UmiAppearanceAppearanceAudit *sample)
{
    (void)sample;
    {
        UmiAppearanceAppearanceAudit invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.audit_id, 'x', sizeof(invalid.audit_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_appearance_audit_is_valid(&invalid)) ||
            umi_appearance_audit_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated audit_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAppearanceAppearanceAuditTransferCases, UmiAppearanceAppearanceAudit,
    umi_appearance_audit_archive_encode, umi_appearance_audit_archive_decode,
    UmiAppearanceAppearanceAuditTransferEqual, UmiAppearanceAppearanceAuditTransferTails, UmiAppearanceAppearanceAuditTransferMalformed)

int main(void) {
    UmiAppearanceAppearanceAudit item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_audit_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_audit_is_valid(&item)) return 2;
    if (UmiAppearanceAppearanceAuditTransferCases(&item) != 0) return 1;

    item.errors=1U; umi_appearance_audit_evaluate(&item); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(item.passed) return 3;
    return 0;
}
