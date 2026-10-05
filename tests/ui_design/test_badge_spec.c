/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_badge_spec.c
 *
 * PURPOSE:
 *   Verify the semantic badge spec contract.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
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
#include "umicom/ui/design/badge_spec.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/design/badge_spec.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDesignBadgeSpecTransferEqual(const UmiDesignBadgeSpec *a, const UmiDesignBadgeSpec *b)
{
    return strcmp(a->text, b->text) == 0 &&
        a->role == b->role &&
        a->outlined == b->outlined;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDesignBadgeSpecTransferTails(UmiDesignBadgeSpec *value)
{
    (void)value;
    {
        size_t used = strlen(value->text) + 1U;
        memset(value->text + used, 0xa5, sizeof(value->text) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDesignBadgeSpecTransferMalformed(const UmiDesignBadgeSpec *sample)
{
    (void)sample;
    {
        UmiDesignBadgeSpec invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.text, 'x', sizeof(invalid.text));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_design_badge_spec_valid(&invalid)) ||
            umi_design_badge_spec_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated text was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDesignBadgeSpecTransferCases, UmiDesignBadgeSpec,
    umi_design_badge_spec_archive_encode, umi_design_badge_spec_archive_decode,
    UmiDesignBadgeSpecTransferEqual, UmiDesignBadgeSpecTransferTails, UmiDesignBadgeSpecTransferMalformed)

int main(void){UmiDesignBadgeSpec s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_design_badge_spec_init(&s,"Ready",UMI_DESIGN_ROLE_SUCCESS,0)!=UMI_STATUS_OK)return 1;
    if (UmiDesignBadgeSpecTransferCases(&s) != 0) return 1;
return s.text[0]=='R'?0:2;}
